#pragma once
#ifndef JARBOCUL_GRAPH_BLOCK_HXX_
# define JARBOCUL_GRAPH_BLOCK_HXX_

# include "block.hh"

static boost::uuids::random_generator uuid_gen = boost::uuids::random_generator();

template <typename CPU>
Block<CPU>::Block(Instruction<CPU>* inst)
    : _insts ()
    , _id ()
    , _block_type (BLOCKTYPE_LOC)
    , _uniq (true)
    , _uniq_id (0)
    , _parents ()
    , _mergeable (true)
{
    this->_insts.push_back(inst);
}

template <typename CPU>
Block<CPU>::~Block()
{}

template <typename CPU>
std::string Block<CPU>::name() const {
    std::ostringstream result;
    std::ostringstream addr;

    uint16_t addr_width = sizeof (typename cpu_traits<CPU>::AddrType) * 2;

    addr << std::setfill('0') << std::setw(addr_width) << std::hex
         << std::uppercase << this->pc();

    result << blocktype2str(this->_block_type) << this->_sep() << addr.str();
    if (!this->_uniq) {
        result << "-" << this->_uniq_id;
    }
    return result.str();
}

template <typename CPU>
BlockId Block<CPU>::id() {
    if (this->_id.is_nil()) {
        this->_id = uuid_gen();
    }
    return this->_id;
}

template <typename CPU>
blocktype Block<CPU>::block_type() const {
    return this->_block_type;
}

template <typename CPU>
std::list<std::string> Block<CPU>::parents() const {
    return this->_parents;
}

template <typename CPU>
inline Instruction<CPU>* Block<CPU>::op() {
    return this->_insts.front();
}

template <typename CPU>
inline Instruction<CPU>* Block<CPU>::op(int _idx) {
    assert(_idx == -1);
    return this->_insts.back();
#if 0
    unsigned long idx;

    if (_idx < 0) {
        idx = this->_insts.size() + _idx;
    } else {
        idx = _idx;
    }
    assert(0 <= idx && idx < this->_insts.size());
    return this->_insts[idx];
#endif
}

template <typename CPU>
bool Block<CPU>::mergeable() const {
    return this->_mergeable;
}

template <typename CPU>
typename cpu_traits<CPU>::AddrType Block<CPU>::pc() const {
    return this->_insts.front()->pc();
}

template <typename CPU>
void Block<CPU>::set_uniq_id(uint32_t uniq_id) {
    this->_uniq = false;
    this->_uniq_id = uniq_id;
}

template <typename CPU>
void Block<CPU>::set_block_type(blocktype block_type) {
    this->_block_type = block_type;
}

template <typename CPU>
void Block<CPU>::set_mergeable(bool mergeable) {
    this->_mergeable = mergeable;
}

template <typename CPU>
void Block<CPU>::add_parent(std::string parent) {
    this->_parents.push_back(parent);
}

template <typename CPU>
void Block<CPU>::merge(Block<CPU>* other) {
    assert(this != other);
    for (Instruction<CPU>* inst : other->_insts) {
        this->_insts.push_back(inst);
    }
}

template <typename CPU>
std::string SpecialBlock<CPU>::name() const {
    return Block<CPU>::name() + "S";
}

#endif /* !JARBOCUL_GRAPH_BLOCK_HXX_ */
