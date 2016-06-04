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
bool link_sorter_froms(Link<CPU>* a, Link<CPU>* b) {
    return a->from()->pc() < b->from()->pc();
}

template <typename CPU>
std::list<Block<CPU>*> Graph<CPU>::functions() {
    return this->_functions;
}

template <typename CPU>
LinkMgr<CPU>& Graph<CPU>::link_mgr() {
    return this->_link_mgr;
}

template <typename CPU>
void Graph<CPU>::generate_graph() {
    const char* line;

    BlockType* last_block;
    BlockType* current_block;

    LinkType* link;

    InstructionType* op;

    /**************************************************************************
     ***** STEP 1: Fetch the graph from the log file.                     *****
     **************************************************************************/

    // Create special block for the beginning of the logs.
    this->_begin = new SpecialLabelBlock<CPU>("BEGIN");
    this->_begin->set_block_type(BLOCKTYPE_SUB);
    this->_begin->set_sep("_1");

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
        for (BlockType* known : this->_blocks[op->pc()]) {
            if (*op == *known->op()) {
                current_block = known;
                break;
            }
        }
        if (current_block == nullptr) {
            current_block = new BlockType(op);
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
                BacktraceItem<CPU>* bt_item = this->_backtrace.top();
                if (
                    bt_item->size() == 0 ||
                    current_block->pc() == bt_item->last_block()->pc() + bt_item->size() ||
                    cpu_functions<CPU>::is_interrupt(op)
                ) {
                    last_block = bt_item->last_block();
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
                this->_backtrace.push(last_block, 3);
            } else {
                link->set_link_type(LINKTYPE_NOT_TAKEN);
            }
            break;

        case jarbocul::lib::flowtype::FLOWTYPE_OPCODE_JUMP:
            if (ft.taken()) {
                if (!this->_link_mgr.triggering_link_found(last_block)) {
                    link->set_link_type(LINKTYPE_TAKEN);
                    this->_link_mgr.set_triggering_link_found(last_block);
                }
            } else {
                link->set_link_type(LINKTYPE_NOT_TAKEN);
            }
            break;
        };

        if (cpu_functions<CPU>::is_interrupt(op)) {
            // Block type is an interrupt.
            current_block->set_block_type(BLOCKTYPE_INT);
            // If the block is an opcode that calls an interrupt, then we set
            // the size to the size of that opcode instead of 0.
            size_t size = cpu_functions<CPU>::interrupt_call_opcode_size(last_block->op());
            // Backtrace must be updated while we are in the interrupt.
            this->_backtrace.push(last_block, size);
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

    this->_end = new SpecialLabelBlock<CPU>("END");
    this->_end->set_sep("_2");

    this->_link_mgr.find_link(last_block, this->_end, true);

    /***************************************************************************
     ***** STEP 2: We now split all calls and only put little boxes,       *****
     *****         unmergeable, that will only contain the name of the     *****
     *****         functions.                                              *****
     **************************************************************************/
    std::list<BlockType*> functions;

    functions.push_back(this->_begin);

    for (std::pair<AddrType, std::list<BlockType*> > _ : this->_blocks) {
        for (BlockType* block : _.second) {
            // If we did interrupts correctly, we don't have any link that come
            // to it, we just need to put it in the function list.
            if (block->block_type() == BLOCKTYPE_INT)
                functions.push_back(block);

            // We only care about subs from here.
            if (block->block_type() != BLOCKTYPE_SUB)
                continue;

            std::vector<LinkType*> links = set_sorter(
                this->_link_mgr.get_all_links_to_block(block),
                link_sorter_froms<CPU>
            );

            size_t idx = 0;

            // For each link, if it's a call link (it is marked as "taken"),
            // then we remove this link and place a little box instead, to be
            // able to split the functions' graphs in multiple files.
            for (LinkType* link : links) {
                if (link->link_type() == LINKTYPE_CALL_TAKEN) {
                    std::stringstream ss;
                    ss << "Call to " << block->name() << ".";

                    BlockType* call_block = new SpecialLabelBlock<CPU>(ss.str());
                    call_block->set_mergeable(false);
                    call_block->op()->set_pc(block->op()->pc());
                    call_block->set_uniq_id(idx);

                    LinkType* call_link = this->_link_mgr.find_link(
                            link->from(), call_block, true);
                    call_link->set_link_type(LINKTYPE_CALL_TAKEN);

                    this->_link_mgr.do_unlink(link);
                }
                idx += 1;
            }

            functions.push_back(block);
         }
     }

    /**************************************************************************
     ***** STEP 3: Now we will merge all the blocks that can be merged to *****
     *****         remove useless links and make it ready.                *****
     **************************************************************************/
    std::list<uint16_t> keys;
    for (std::pair<uint16_t, std::list<BlockType*> > _ : this->_blocks) {
        keys.push_back(_.first);
    }
    keys.sort();

    for (uint16_t addr : keys) {
        this->_merge_blocks(this->_begin);
        for (BlockType* block : this->_blocks[addr]) {
            this->_merge_blocks(block);
        }
    }

    /**************************************************************************
     ***** STEP 4: Now we decide  which functions we will need to         *****
     *****         generate.                                              *****
     **************************************************************************/
    std::list<BlockType*> tmp_inner_functions;
    std::list<BlockType*> res_inner_functions;

    for (BlockType* func : functions) {
        // We have two possibilities: the beginning of the sub is not only
        // called, so we keep it for later concidering it to be within another
        // function. Else, we just cut out the current sub function from the
        // blocks.
        std::set<LinkType*> links = this->_link_mgr.get_all_links_to_block(func);
        if (links.size() != 0) {
            tmp_inner_functions.push_back(func);
        } else {
            this->_functions.push_back(func);
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
    for (BlockType* inner : tmp_inner_functions) {
        if (inner->parents().size() == 0) {
            this->_functions.push_back(inner);
            this->_cutfunction(inner);
        } else {
            res_inner_functions.push_back(inner);
        }
    }

    std::cout << "Found " << this->_functions.size() << " functions." << std::endl;
    std::cout << "Found " << res_inner_functions.size() << " inner functions." << std::endl;

    for (BlockType* func : this->_functions) {
        std::cout << " - " << func->name() << std::endl;
    }
    for (BlockType* func : res_inner_functions) {
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
}

template <typename CPU>
void Graph<CPU>::_cutfunction(BlockType* func) {
    std::queue<BlockType*> todos;
    std::set<BlockId> done;

    todos.push(func);
    while (!todos.empty()) {
        BlockType* todo = todos.front();
        todos.pop();

        if (done.count(todo->id()) != 0) {
            continue;
        }
        done.insert(todo->id());

        this->_blocks[todo->pc()].remove(todo);

        todo->add_parent(func->name());

        for (LinkType* link : this->_link_mgr.get_all_links_from_block(todo)) {
            todos.push(link->to());
        }
    }
}

template <typename CPU>
void Graph<CPU>::_merge_blocks(BlockType* block) {
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
        std::set<LinkType*> dests = this->_link_mgr.get_all_links_from_block(block);
        assert(dests.size() == 1);
        LinkType* link_to = *dests.begin();
        BlockType* to = link_to->to();

        if (!to->mergeable()) {
            break;
        }

        if (!this->_link_mgr.accepts_merge_top(to)) {
            break;
        }

        this->_blocks[to->pc()].remove(to);

        block->merge(to);

        for (LinkType* link_to_merge : this->_link_mgr.get_all_links_from_block(to)) {
            LinkType* link_tmp = this->_link_mgr.find_link(block, link_to_merge->to(), true);
            link_tmp->set_link_type(link_to_merge->link_type());

            this->_link_mgr.do_unlink(link_to_merge);
        }
        this->_link_mgr.do_unlink(link_to);
    }
}

#endif /*!JARBOCUL_GRAPH_GRAPH_HXX_*/
