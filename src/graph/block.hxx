#ifndef BLOCK_HXX_
# define BLOCK_HXX_

template <typename CPU>
Block::Block(Instruction<CPU>* inst)
    : _block_type (BLOCKTYPE_LOC)
    , _uniq (true)
    , _uniq_id (0)
    , _insts ()
    , _within ()
    , _mergeable (true)
{
    this->_insts.push_back(inst);
}

template <typename CPU>
Block::~Block()
{}

template <typename CPU>
std::string Block::name() {
    std::ostringstream result;
    std::ostringstream addr;

    uint16_t addr_width = sizeof (cpu_traits<CPU>::Addr) * 2;

    addr << std::setfill('0') << std::setw(addr_width) << std::hex
         << std::uppercase << this->pc();

    result << blocktype2str(this->_block_type) << this->_sep() << addr.str();
    if (!this->_uniq) {
        result << "-" << this->_uniq_id;
    }
    return result.str();
}

template <typename CPU>
Instruction<CPU>* Block::op() {
    assert(this->_insts.size() == 1);
    return this->_insts.front();
}

template <typename CPU>
cpu_traits<CPU>::Addr pc() {
    return this->_insts.front()->pc();
}

template <typename CPU>
void Block::set_uniq_id(uint32_t uniq_id) {
    this->_uniq = false;
    this->_uniq_id = uniq_id;
}

template <typename CPU>
void Block::set_block_type(blocktype block_type) {
    this->_block_type = block_type;
}

template <typename CPU>
void Block::merge(Block<CPU>* other) {
    assert(this != other);
    for (Instruction<CPU>* inst : other->_insts) {
        this->_insts.push_back(inst);
    }
}

template <typename CPU>
std::string SpecialBlock::name() {
    return Block<CPU>::name() + "S";
}

#endif /* !BLOCK_HXX_ */
