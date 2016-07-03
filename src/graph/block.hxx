#pragma once
#ifndef JARBOCUL_GRAPH_BLOCK_HXX_
# define JARBOCUL_GRAPH_BLOCK_HXX_

# include "graph/block.hh"

static boost::uuids::random_generator uuid_gen = boost::uuids::random_generator();

template <typename CPU>
Block<CPU>::Block(Instruction<CPU>* inst)
    : _sep ("_")
    , _insts ()
    , _id (uuid_gen())
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

    result << blocktype2str(this->_block_type)
           << this->_sep
           << addr2str<CPU>(this->pc());
    return result.str();
}

template <typename CPU>
std::string Block<CPU>::uniq_name() const {
    std::ostringstream result;

    result << this->name();
    if (!this->_uniq) {
        result << "_" << this->_uniq_id;
    }
    return result.str();
}

template <typename CPU>
BlockId Block<CPU>::id() const {
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
    unsigned long idx = _idx;

    if (_idx < 0) {
        idx = this->_insts.size() + _idx;
    }
    assert(0 <= idx && idx < this->_insts.size());
    return this->_insts[idx];
}

template <typename CPU>
uint32_t Block<CPU>::uniq_id() const {
    return this->_uniq_id;
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
void Block<CPU>::set_sep(std::string sep) {
    this->_sep = sep;
}

template <typename CPU>
void Block<CPU>::merge(Block<CPU>* other) {
    assert(this != other);
    for (Instruction<CPU>* inst : other->_insts) {
        this->_insts.push_back(inst);
    }
}

template <typename CPU>
void Block<CPU>::_ostream_write(std::ostream& os) const {
    if (this->_mergeable) {
        os << this->name() << ":\n";
    }
    for (Instruction<CPU>* inst : this->_insts) {
        os << *inst << "\n";
    }
}

# define _PADDING_SIZE (sizeof (typename cpu_traits<CPU>::AddrType) * 2 + 5)

template <typename CPU>
void SpecialInstruction<CPU>::_ostream_write(std::ostream& os) const {
    std::string padding(_PADDING_SIZE, ' ');

    if (this->_block->mergeable()) {
        os << padding;
    }
    os << this->_label;
}

template <typename CPU>
SpecialLabelBlock<CPU>::SpecialLabelBlock(std::string label)
    : Block<CPU>(nullptr)
{
    this->_insts[0] = new SpecialInstruction<CPU>(this, label);
}

template <typename CPU>
std::string SpecialLabelBlock<CPU>::name() const {
    return Block<CPU>::name() + "S";
}

template <typename CPU>
void SpecialLabelBlock<CPU>::_ostream_write(std::ostream& os) const {
    std::string                 item;
    std::stringstream           ss;
    std::vector<std::string>    lines;

    Block<CPU>::_ostream_write(ss);

    while (std::getline(ss, item, '\n')) {
        lines.push_back(item);
    }

    if (lines.size() == 2) {
        os << lines[1] << '\n';
    } else {
        os << ss.str();
    }
}

#endif /* !JARBOCUL_GRAPH_BLOCK_HXX_ */
