#pragma once
#ifndef BLOCK_HH_
# define BLOCK_HH_

# include <iomanip>
# include <list>
# include <sstream>
# include <string>

# include <assert.h>

# include "instruction.hh"

typedef enum blocktype_e {
    BLOCKTYPE_INT   = 0x01,
    BLOCKTYPE_LOC   = 0x02,
    BLOCKTYPE_SUB   = 0x03,
} blocktype;

std::string blocktype2str(blocktype t);

typedef uint64_t BlockId;

template <typename CPU>
class Block {
public:
    Block(Instruction<CPU>* inst);
    virtual ~Block();

    virtual std::string name();
    // virtual BlockId id();

    void merge(Block<CPU>* other);

    Instruction<CPU>* op();

    cpu_traits<CPU>::Addr pc();

    void set_uniq_id(uint32_t uniq_id);
    void set_block_type(blocktype block_type);

private:
    virtual const char* _sep()  { return "_"; };

private:
    blocktype _block_type;

    std::list<Instruction<CPU>*> _insts;

    bool _uniq;
    uint32_t _uniq_id;

    std::list<std::string> _within;

    bool _mergeable;
};

template <typename CPU>
class SpecialBlock : public Block<CPU> {
public:
    SpecialBlock() : Block(new Instruction<CPU>()) {
        Instruction<CPU>* instr = this->insts.front();

        instr->set_pc(0);
    }

    std::string name();

private:
    const char* _sep()  { return "_1"; };
};

# include "block.hxx"

#endif /* !BLOCK_HH_ */
