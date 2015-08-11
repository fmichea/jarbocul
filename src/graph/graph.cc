#include "graph.hh"



FileReader::FileReader(std::string filename)
    : _allocated_data (nullptr)
    , _offset (0)
{
    struct stat st;

    this->_fd = open(filename.c_str(), O_RDONLY);

    fstat(this->_fd, &st);
    this->_size = st.st_size;

    void* data = mmap(NULL, this->_size, PROT_READ, MAP_SHARED, this->_fd, 0);
    this->_data = static_cast<char*>(data);
}

FileReader::~FileReader() {
    if (this->_allocated_data != nullptr)
        delete this->_allocated_data;
    munmap(this->_data, this->_size);
    close(this->_fd);
}

bool FileReader::eof() {
    return this->_offset == this->_size;
}

const char* FileReader::readline() {
    std::string result;

    if (this->_allocated_data != nullptr) {
        delete this->_allocated_data;
        this->_allocated_data = nullptr;
    }

    const char* start = this->_data + this->_offset;
    size_t len = this->_size - this->_offset;

    const char* end = static_cast<const char*>(memchr(start, '\n', len));
    if (end != nullptr) {
        len = static_cast<size_t>(end - start);
    }

    char* res = this->_rw_data;
    if (FileReader::RW_DATA_SZ < len) {
        this->_allocated_data = new char[len + 1];
        res = this->_allocated_data;
    }
    memcpy(res, start, len);
    res[len] = 0;

    this->_offset += len + 1;
    return res;
}

template <typename CPU>
Graph<CPU>::Graph<CPU>(std::string filename)
    : _file (filename)
{}

template <typename CPU>
Graph<CPU>::~Graph<CPU>() {}

template <typename CPU>
void Graph<CPU>::generate_graph() {
    const char* line;

    std::stack<std::pair<Block<CPU>*, cpu_traits<CPU>::Addr>> backtrace;

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

        op = parse_line<CPU>(line);

        // If line is not recognized, just skip it.
        if (op == nullptr)
            continue;

        /* Check if we already know the current instruction for the current
        ** program counter. If we do, we keep the current block and add a
        ** link. */
        current_block = nullptr;
        for (Block* known : blocks[op->pc()]) {
            if (op->op() == known->op()) {
                current_block = known;
                break;
            }
        }
        if (current_block == nullptr) {
            current_block = new Block<CPU>(op);
            if (0 < blocks[op->pc()].size()) {
                /* No loop needed, if we set the first one and each one
                ** starting from the second, we will set them all. */
                current_block->set_uniq_id(blocks.count(op->pc));
                blocks[op->pc()].front()->set_uniq_id(0);
            }
            blocks[op->pc()].push_back(current_block);
        }

        // Now we need to link the current block with the last one.
        link = this->_link_mgr.find_link(last_block, current_block);

        // Now we need to treat special cases.
        AddrOffset<CPU> offset(last_block->pc(), current_block->pc());

        const ft_np::FT ft = flowtype<CPU>(last_block, offset);

        switch (ft.opcode_type()) {
        case ft_np::FLOWTYPE_OPCODE_NONE:
            break;

        case ft_np::FLOWTYPE_OPCODE_RET:
            if (!ft.taken())
                break;

            /* We have a ret, and it triggered. In that case, we will try to
            ** use our backtrace to go where we were called. */
            if (!backtrace.empty()) {
                Block* back = backtrace.top().first;
                uint16_t size = backtrace.top().second;
                if (size == 0 || current_block->pc == back->pc + size || is_interrupt(op)) {
                    last_block = back;
                    //delete link;
                    link = this->_link_mgr.find_link(last_block, current_block);
                    backtrace.pop();
                    break;
                }
            }
            /* If we reached this point, it means that the backtrace didn't
            ** point to any place where we didn't come with a call from. In
            ** that case, we set the link as a return miss link. */
            link->link_type = LINKTYPE_RET_MISS;
            break;

        case ft_np::FLOWTYPE_OPCODE_CALL:
            if (ft.taken()) {
                if (!last_block->tlf) {
                    current_block->block_type = BLOCKTYPE_SUB;
                    link->link_type = LINKTYPE_CALL_TAKEN;
                    last_block->tlf = true;
                }
                backtrace.push(std::pair<Block*, uint16_t>(last_block, 3));
            } else {
                link->link_type = LINKTYPE_NOT_TAKEN;
            };
            break;

        case ft_np::FLOWTYPE_OPCODE_JUMP:
            if (ft.taken()) {
                if (!last_block->tlf) {
                    link->link_type = LINKTYPE_TAKEN;
                    last_block->tlf = true;
                }
            } else {
                link->link_type = LINKTYPE_NOT_TAKEN;
            };
            break;
        };

        if (is_interrupt(op)) {
            uint16_t size = 0;
            // Block type is an interrupt.
            current_block->block_type = BLOCKTYPE_INT;
            // If the block is an opcode that calls an interrupt, then we set
            // the size to the size of that opcode instead of 0.
            for (uint16_t tmp = 0xC7; tmp < 0x100; tmp += 0x8) {
                if (tmp == last_block->op()->opcode) {
                    size = 1;
                    break;
                }
            }
            // Backtrace must be updated while we are in the interrupt.
            backtrace.push(std::pair<Block*, uint16_t>(last_block, size));
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

    this->_end = new SpecialBlock();
    this->_link_mgr.find_link(last_block, this->_end, true);

    /***************************************************************************
     ***** STEP 2: We now split all calls and only put little boxes,       *****
     *****         unmergeable, that will only contain the name of the     *****
     *****         functions.                                              *****
     **************************************************************************/
    std::list<Block*> functions;

    functions.push_back(this->_begin);
    for (std::pair<uint16_t, std::list<Block*> > _ : blocks) {
        for (Block* block : _.second) {
            // If we did interrupts correctly, we don't have any link that come
            // to it, we just need to put it in the function list.
            if (block->block_type == BLOCKTYPE_INT)
                functions.push_back(block);

            // We only care about subs from here.
            if (block->block_type != BLOCKTYPE_SUB)
                continue;

            // For each link, if it's a call link (it is marked as "taken"),
            // then we remove this link and place a little box instead, to be
            // able to split the functions' graphs in multiple files.
            for (Link* link : this->_link_mgr.get_all_links_to_block(block)) {
                if (link->link_type != LINKTYPE_CALL_TAKEN)
                    continue;
                Block* call_block = new SpecialBlock();
                call_block->mergeable = false;
                call_block->op()->pc = block->op()->pc;
                // FIXME: call_block->set_mergeable(false);
                Link* call_link = this->_link_mgr.find_link(link->from, call_block, true);
                call_link->link_type = LINKTYPE_CALL_TAKEN;
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
    for (std::pair<uint16_t, std::list<Block*> > _ : blocks) {
        keys.push_back(_.first);
    }
    keys.sort();

    for (uint16_t addr : keys) {
        for (Block* block : blocks[addr]) {
            while (true) {
                if (!block->mergeable) {
                    break;
                }

                bool is_flow_opcode = false;
                switch (block->insts.back()->opcode) {
                // ret
                case 0xC0: case 0xC8: case 0xC9:
                case 0xD0: case 0xD8: case 0xD9:
                // call
                case 0xC4: case 0xCC: case 0xCD:
                case 0xD4: case 0xDC:
                // jump
                case 0xC2: case 0xC3: case 0xCA:
                case 0xD2: case 0xDA:
                case 0xE9:
                // jr
                case 0x18:
                case 0x20: case 0x28:
                case 0x30: case 0x38:
                    is_flow_opcode = true;
                    break;

                default:
                    break;
                };
                if (is_flow_opcode) {
                    break;
                }

                // If this block cannot be merged on its bottom, we ignore it.
                if (!this->_link_mgr.accepts_merge_bottom(block)) {
                    break;
                }

                // We now know that we have only one link, we fetch the block
                // it goes to and check whether it accepts top merges too.
                std::set<Link*> dests = this->_link_mgr.get_all_links_from_block(block);
                assert(dests.size() == 1);
                Link* link_to = *dests.begin();
                Block* to = link_to->to;

                if (!to->mergeable) {
                    break;
                }

                if (!this->_link_mgr.accepts_merge_top(to)) {
                    break;
                }

                blocks[to->pc].remove(to);

                block->merge(to);

                for (Link* link_to_merge : this->_link_mgr.get_all_links_from_block(to)) {
                    this->_link_mgr.find_link(block, link_to_merge->to, true);
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
    std::list<Block*> res_functions;

    std::list<Block*> tmp_inner_functions;
    std::list<Block*> res_inner_functions;

    for (Block* func : functions) {
        // We have two possibilities: the beginning of the sub is not only
        // called, so we keep it for later concidering it to be within another
        // function. Else, we just cut out the current sub function from the
        // blocks.
        std::set<Link*> links = this->_link_mgr.get_all_links_to_block(func);
        if (links.size() != 0) {
            tmp_inner_functions.push_back(func);
        } else {
            res_functions.push_back(func);
            cutfunction(this->_link_mgr, blocks, func);
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
    for (Block* inner : tmp_inner_functions) {
        if (inner->within.size() == 0) {
            res_functions.push_back(inner);
            cutfunction(this->_link_mgr, blocks, inner);
        } else {
            res_inner_functions.push_back(inner);
        }
    }

    std::cout << "Found " << res_functions.size() << " functions." << std::endl;
    std::cout << "Found " << res_inner_functions.size() << " inner functions." << std::endl;

    for (Block* func : res_functions) {
        std::cout << " - " << func->name() << std::endl;
    }
    for (Block* func : res_inner_functions) {
        bool first = true;

        std::cout  << " - " << func->name() << " within the functions ";
        for (std::string block_within : func->within) {
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

    for (Block* func : res_functions) {
        char filename[256];
        char tmp[256];
        snprintf(filename, 256, "result/%s.dot", func->name().c_str());

        std::ofstream out(filename);
        out << "digraph " << func->name() << " {" << std::endl;
        out << "\tsplines = true;" << std::endl;
        out << "\tnode [ shape = box, fontname = \"Monospace\" ];" << std::endl << std::endl;

        std::queue<Block*> blocks_to_output;
        std::set<BlockId> done;

        blocks_to_output.push(func);
        while (!blocks_to_output.empty()) {
            Block* func_block = blocks_to_output.front();
            blocks_to_output.pop();

            if (done.count(func_block->id()) != 0) {
                continue;
            }
            done.insert(func_block->id());

            out << "\t" << func_block->name() << " [ label = \"";
            out << func_block->name() << ":\\l";
            for (Instruction* inst : func_block->insts) {
                snprintf(tmp, 256, "     %04X: %02X - %s\\l", inst->pc, inst->opcode, disassemble(inst));
                out << tmp;
            }
            out << "\" ];" << std::endl;

            for (Link* link : this->_link_mgr.get_all_links_from_block(func_block)) {
                out << "\t" << link->from->name() << " -> " << link->to->name()
                    << " [ tailport = s, headport = n ];" << std::endl;
                blocks_to_output.push(link->to);
            }
            out << std::endl;
        }

        out << "}" << std::endl;
        out.close();
    }
}

template <typename CPU>
void cutfunction(Block<CPU>* func) {
    std::queue<Block<CPU>*> todos;
    std::set<BlockId> done;

    todos.push(func);
    while (!todos.empty()) {
        Block* todo = todos.front();
        todos.pop();

        if (done.count(todo->id()) != 0) {
            continue;
        }
        done.insert(todo->id());

        blocks[todo->pc].remove(todo);

        todo->within.push_back(func->name());

        for (Link* link : link_mgr.get_all_links_from_block(todo)) {
            todos.push(link->to);
        }
    }
}
