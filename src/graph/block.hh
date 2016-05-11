#pragma once
#ifndef JARBOCUL_GRAPH_BLOCK_HH_
# define JARBOCUL_GRAPH_BLOCK_HH_

# include <iomanip>
# include <iostream>
# include <list>
# include <sstream>
# include <string>
# include <vector>

# include <assert.h>

# include <boost/uuid/uuid.hpp>
# include <boost/uuid/uuid_generators.hpp>

# include "instruction.hh"

typedef enum {
    BLOCKTYPE_INT   = 0x01,
    BLOCKTYPE_LOC   = 0x02,
    BLOCKTYPE_SUB   = 0x03,
} blocktype;

std::string blocktype2str(blocktype t);

typedef boost::uuids::uuid BlockId;

template <typename CPU>
class Block {
public:
    Block(Instruction<CPU>* inst);
    virtual ~Block();

    virtual std::string name() const;
    BlockId id() const;
    blocktype block_type() const;
    bool mergeable() const;
    std::list<std::string> parents() const;

    void merge(Block<CPU>* other);

    Instruction<CPU>* op();
    Instruction<CPU>* op(int idx);

    typename cpu_traits<CPU>::AddrType pc() const;

    void set_uniq_id(uint32_t uniq_id);
    void set_block_type(blocktype block_type);
    void set_mergeable(bool mergeable);
    void add_parent(std::string block);

public:
    template <typename T>
    friend std::ostream& operator << (std::ostream& os, const Block<T>& block);

private:
    virtual const char* _sep() const { return "_"; };

protected:
    std::vector<Instruction<CPU>*> _insts;

private:
    BlockId _id;

    blocktype _block_type;

    bool _uniq;
    uint32_t _uniq_id;

    std::list<std::string> _parents;

    bool _mergeable;
};

template <typename CPU>
std::ostream& operator << (std::ostream& os, const Block<CPU>& block);

template <typename CPU>
class SpecialInstruction : public Instruction<CPU> {
public:
    SpecialInstruction(Block<CPU>* block, std::string label)
        : Instruction<CPU>()
        , _block (block)
        , _label (label)
    {}

    void _ostream_write(std::ostream& os) const;

private:
    Block<CPU>* _block;
    std::string _label;
};

# define _PADDING_SIZE (sizeof (typename cpu_traits<CPU>::AddrType) * 2 + 4 + 2)

template <typename CPU>
void SpecialInstruction<CPU>::_ostream_write(std::ostream& os) const {
    std::string padding(_PADDING_SIZE, ' ');

    if (this->_block->mergeable()) {
        os << padding;
    }
    os << this->_label;
}

template <typename CPU>
class SpecialLabelBlock : public Block<CPU> {
public:
    SpecialLabelBlock(std::string label)
        : Block<CPU>(nullptr)
//        , _label (label)
    {
        this->_insts[0] = new SpecialInstruction<CPU>(this, label);
    }

    std::string name() const;

//public:
//    template <typename CPU>
//    friend std::ostream& operator << (std::ostream& os, const SpecialBlock<CPU>& block);

private:
    const char* _sep() const { return "_1"; };

//private:
//    std::string _label;
};

//template <typename CPU>
//std::ostream& operator << (std::ostream& os, const SpecialBlock<CPU>& block) {
//    os << block._label << std::endl;
//    return os;
//}

# include "block.hxx" 
#endif /* !JARBOCUL_GRAPH_BLOCK_HH_ */
