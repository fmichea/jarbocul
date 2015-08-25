#pragma once
#ifndef JARBOCUL_GRAPH_GRAPH_HXX_
# define JARBOCUL_GRAPH_GRAPH_HXX_

# include "graph.hh"

template <typename CPU>
Graph<CPU>::Graph(std::string filename)
    : _file (filename)
    , _link_mgr ()
    , _begin (nullptr)
    , _end (nullptr)
    , _blocks ()
    , _backtrace ()
{}

template <typename CPU>
Graph<CPU>::~Graph()
{}

template <typename CPU>
void Graph<CPU>::generate_graph() {
    const char* line;

    Block<CPU>* last_block;
    Block<CPU>* current_block;

    Link<CPU>* link;

    Instruction<CPU>* op;

    /**************************************************************************
     ***** STEP 1: Fetch the graph from the log file.                     *****
     **************************************************************************/

    // Create special block for the beginning of the logs.
    this->_begin = new SpecialBlock<CPU>();
    this->_begin->set_block_type(BLOCKTYPE_SUB);

    // Use the first block as first "last_block".
    last_block = this->_begin;

    while (!this->_file.eof()) {
        line = this->_file.readline();

        op = cpu_functions<CPU>::parse_line(line);

        // If line is not recognized, just skip it.
        if (op == nullptr)
            continue;

        /* Check if we already know the current instruction for the current
        ** program counter. If we do, we keep the current block and add a
        ** link. */
        current_block = nullptr;
        for (Block<CPU>* known : this->_blocks[op->pc()]) {
            if (op == known->op()) {
                current_block = known;
                break;
            }
        }
        if (current_block == nullptr) {
            current_block = new Block<CPU>(op);
            if (0 < this->_blocks[op->pc()].size()) {
                /* No loop needed, if we set the first one and each one
                ** starting from the second, we will set them all. */
                current_block->set_uniq_id(this->_blocks.count(op->pc()));
                this->_blocks[op->pc()].front()->set_uniq_id(0);
            }
            this->_blocks[op->pc()].push_back(current_block);
        }

        // Now we need to link the current block with the last one.
        link = this->_link_mgr.find_link(last_block, current_block);

        // Now we need to treat special cases.
        AddrOffset<CPU> offset(last_block->pc(), current_block->pc());

        const jarbocul::lib::flowtype::FT ft = cpu_functions<CPU>::flowtype(last_block, offset);

        switch (ft.opcode_type()) {
        case jarbocul::lib::flowtype::FLOWTYPE_OPCODE_NONE:
            break;

        case jarbocul::lib::flowtype::FLOWTYPE_OPCODE_RET:
            if (!ft.taken())
                break;

            /* We have a ret, and it triggered. In that case, we will try to
            ** use our backtrace to go where we were called. */
            if (!this->_backtrace.empty()) {
                Block<CPU>* back = this->_backtrace.top().first;
                uint16_t size = this->_backtrace.top().second;
                if (size == 0 || current_block->pc() == back->pc() + size ||
                    cpu_functions<CPU>::is_interrupt(op)) {
                    last_block = back;
                    //delete link;
                    link = this->_link_mgr.find_link(last_block, current_block);
                    this->_backtrace.pop();
                    break;
                }
            }
            /* If we reached this point, it means that the backtrace didn't
            ** point to any place where we didn't come with a call from. In
            ** that case, we set the link as a return miss link. */
            link->set_link_type(LINKTYPE_RET_MISS);
            break;

        case jarbocul::lib::flowtype::FLOWTYPE_OPCODE_CALL:
            if (ft.taken()) {
                if (!this->_link_mgr.triggering_link_found(last_block)) {
                    current_block->set_block_type(BLOCKTYPE_SUB);
                    link->set_link_type(LINKTYPE_CALL_TAKEN);
                    this->_link_mgr.set_triggering_link_found(last_block);
                }
                this->_backtrace.push(std::pair<Block<CPU>*, uint16_t>(last_block, 3));
            } else {
                link->set_link_type(LINKTYPE_NOT_TAKEN);
            };
            break;

        case jarbocul::lib::flowtype::FLOWTYPE_OPCODE_JUMP:
            if (ft.taken()) {
                if (!this->_link_mgr.triggering_link_found(last_block)) {
                    link->set_link_type(LINKTYPE_TAKEN);
                    this->_link_mgr.set_triggering_link_found(last_block);
                }
            } else {
                link->set_link_type(LINKTYPE_NOT_TAKEN);
            };
            break;
        };

        if (cpu_functions<CPU>::is_interrupt(op)) {
            uint16_t size = 0;
            // Block type is an interrupt.
            current_block->set_block_type(BLOCKTYPE_INT);
            // If the block is an opcode that calls an interrupt, then we set
            // the size to the size of that opcode instead of 0.
            if (cpu_functions<CPU>::is_interrupt_call(last_block->op())) {
                size = 1; // FIXME
            }
            // Backtrace must be updated while we are in the interrupt.
            this->_backtrace.push(std::pair<Block<CPU>*, uint16_t>(last_block, size));
            // We can get rid of the link now.
            delete link;
            link = nullptr;
        }

        /* We finally really link the Link if it still exists and was not known,
        ** and add the block to the list of blocks. */
        this->_link_mgr.do_link(link);

        // To be used in next step.
        last_block = current_block;
    }

    this->_end = new SpecialBlock<CPU>();
    this->_link_mgr.find_link(last_block, this->_end, true);

    /***************************************************************************
     ***** STEP 2: We now split all calls and only put little boxes,       *****
     *****         unmergeable, that will only contain the name of the     *****
     *****         functions.                                              *****
     **************************************************************************/
    std::list<Block<CPU>*> functions;

    functions.push_back(this->_begin);
    for (std::pair<uint16_t, std::list<Block<CPU>*> > _ : this->_blocks) {
        for (Block<CPU>* block : _.second) {
            // If we did interrupts correctly, we don't have any link that come
            // to it, we just need to put it in the function list.
            if (block->block_type() == BLOCKTYPE_INT)
                functions.push_back(block);

            // We only care about subs from here.
            if (block->block_type() != BLOCKTYPE_SUB)
                continue;

            // For each link, if it's a call link (it is marked as "taken"),
            // then we remove this link and place a little box instead, to be
            // able to split the functions' graphs in multiple files.
            for (Link<CPU>* link : this->_link_mgr.get_all_links_to_block(block)) {
                if (link->link_type() != LINKTYPE_CALL_TAKEN)
                    continue;
                Block<CPU>* call_block = new SpecialBlock<CPU>();
                call_block->set_mergeable(false);
                call_block->op()->set_pc(block->op()->pc());
                Link<CPU>* call_link = this->_link_mgr.find_link(link->from(), call_block, true);
                call_link->set_link_type(LINKTYPE_CALL_TAKEN);
                this->_link_mgr.do_unlink(link);
            }

            functions.push_back(block);
         }
     }

    /**************************************************************************
     ***** STEP 3: Now we will merge all the blocks that can be merged to *****
     *****         remove useless links and make it ready.                *****
     **************************************************************************/
    std::list<uint16_t> keys;
    for (std::pair<uint16_t, std::list<Block<CPU>*> > _ : this->_blocks) {
        keys.push_back(_.first);
    }
    keys.sort();

    for (uint16_t addr : keys) {
        for (Block<CPU>* block : this->_blocks[addr]) {
            while (true) {
                if (!block->mergeable()) {
                    break;
                }

                if (cpu_functions<CPU>::is_flow_instruction(block->op(-1))) {
                    break;
                }

                // If this block cannot be merged on its bottom, we ignore it.
                if (!this->_link_mgr.accepts_merge_bottom(block)) {
                    break;
                }

                // We now know that we have only one link, we fetch the block
                // it goes to and check whether it accepts top merges too.
                std::set<Link<CPU>*> dests = this->_link_mgr.get_all_links_from_block(block);
                assert(dests.size() == 1);
                Link<CPU>* link_to = *dests.begin();
                Block<CPU>* to = link_to->to();

                if (!to->mergeable()) {
                    break;
                }

                if (!this->_link_mgr.accepts_merge_top(to)) {
                    break;
                }

                this->_blocks[to->pc()].remove(to);

                block->merge(to);

                for (Link<CPU>* link_to_merge : this->_link_mgr.get_all_links_from_block(to)) {
                    this->_link_mgr.find_link(block, link_to_merge->to(), true);
                    this->_link_mgr.do_unlink(link_to_merge);
                }
                this->_link_mgr.do_unlink(link_to);
            }
        }
    }

    /**************************************************************************
     ***** STEP 4: Now we decide  which functions we will need to         *****
     *****         generate.                                              *****
     **************************************************************************/
    std::list<Block<CPU>*> res_functions;

    std::list<Block<CPU>*> tmp_inner_functions;
    std::list<Block<CPU>*> res_inner_functions;

    for (Block<CPU>* func : functions) {
        // We have two possibilities: the beginning of the sub is not only
        // called, so we keep it for later concidering it to be within another
        // function. Else, we just cut out the current sub function from the
        // blocks.
        std::set<Link<CPU>*> links = this->_link_mgr.get_all_links_to_block(func);
        if (links.size() != 0) {
            tmp_inner_functions.push_back(func);
        } else {
            res_functions.push_back(func);
            this->_cutfunction(func);
        }
    }

    // Finally, for each "inner function" that were not reached from any
    // standard function, we cut it out and generate it anyway, it must mean it
    // is "within itself", example:
    //
    // sub_0216:
    //    0216 - ldh %a, ($0xFF44)
    //    0218 - cp %a, $0x145
    //    021A - jr cy, $0xFA ; ($-6)
    //    021C - ret
    for (Block<CPU>* inner : tmp_inner_functions) {
        if (inner->parents().size() == 0) {
            res_functions.push_back(inner);
            this->_cutfunction(inner);
        } else {
            res_inner_functions.push_back(inner);
        }
    }

    std::cout << "Found " << res_functions.size() << " functions." << std::endl;
    std::cout << "Found " << res_inner_functions.size() << " inner functions." << std::endl;

    for (Block<CPU>* func : res_functions) {
        std::cout << " - " << func->name() << std::endl;
    }
    for (Block<CPU>* func : res_inner_functions) {
        bool first = true;

        std::cout  << " - " << func->name() << " within the functions ";
        for (std::string block_within : func->parents()) {
            if (!first) {
                std::cout << ", ";
            }
            std::cout << block_within;
            first = false;
        }
        std::cout << std::endl;
    }

    /**************************************************************************
     ***** STEP 5: SANITY CHECK: if there are still blocks in the main    *****
     *****         map, we probably failed something.                     *****
     **************************************************************************/
    // TODO

    for (Block<CPU>* func : res_functions) {
        char filename[256];
        // FIXME: char tmp[256];
        snprintf(filename, 256, "result/%s.dot", func->name().c_str());

        std::ofstream out(filename);
        out << "digraph " << func->name() << " {" << std::endl;
        out << "\tsplines = true;" << std::endl;
        out << "\tnode [ shape = box, fontname = \"Monospace\" ];" << std::endl << std::endl;

        std::queue<Block<CPU>*> blocks_to_output;
        std::set<BlockId> done;

        blocks_to_output.push(func);
        while (!blocks_to_output.empty()) {
            Block<CPU>* func_block = blocks_to_output.front();
            blocks_to_output.pop();

            if (done.count(func_block->id()) != 0) {
                continue;
            }
            done.insert(func_block->id());

            out << "\t" << func_block->name() << " [ label = \"";
#if 0
            out << func_block->name() << ":\\l";
            for (Instruction<CPU>* inst : func_block->insts) {
                snprintf(tmp, 256, "     %04X: %02X - %s\\l", inst->pc(), inst->opcode, disassemble(inst));
                out << tmp;
            }
#endif
            out << "\" ];" << std::endl;

            for (Link<CPU>* link : this->_link_mgr.get_all_links_from_block(func_block)) {
                out << "\t" << link->from()->name() << " -> " << link->to()->name()
                    << " [ tailport = s, headport = n ];" << std::endl;
                blocks_to_output.push(link->to());
            }
            out << std::endl;
        }

        out << "}" << std::endl;
        out.close();
    }
}

template <typename CPU>
void Graph<CPU>::_cutfunction(Block<CPU>* func) {
    std::queue<Block<CPU>*> todos;
    std::set<BlockId> done;

    todos.push(func);
    while (!todos.empty()) {
        Block<CPU>* todo = todos.front();
        todos.pop();

        if (done.count(todo->id()) != 0) {
            continue;
        }
        done.insert(todo->id());

        this->_blocks[todo->pc()].remove(todo);

        todo->add_parent(func->name());

        for (Link<CPU>* link : this->_link_mgr.get_all_links_from_block(todo)) {
            todos.push(link->to());
        }
    }
}

#endif /*!JARBOCUL_GRAPH_GRAPH_HXX_*/