#pragma once
#ifndef JARBOCUL_GRAPH_BLOCK_HH_
# define JARBOCUL_GRAPH_BLOCK_HH_

# include <iomanip>
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
    BlockId id();
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

private:
    virtual const char* _sep() const { return "_"; };

protected:
    std::list<Instruction<CPU>*> _insts;

private:
    BlockId _id;

    blocktype _block_type;

    bool _uniq;
    uint32_t _uniq_id;

    std::list<std::string> _parents;

    bool _mergeable;
};

template <typename CPU>
class SpecialBlock : public Block<CPU> {
public:
    SpecialBlock() : Block<CPU>(new Instruction<CPU>()) {
        Instruction<CPU>* instr = this->_insts.front();

        instr->set_pc(0);
    }

    std::string name() const;

private:
    const char* _sep() const { return "_1"; };
};

# include "block.hxx"

#endif /* !JARBOCUL_GRAPH_BLOCK_HH_ */
