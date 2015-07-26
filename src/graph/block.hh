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

class Block {
public:
    Block(Instruction* inst);
    virtual ~Block();

    virtual std::string name();
    virtual BlockId id() const;

    void merge(Block* other);

#if 0
    void set_uniq(bool uniq);
    void set_uniq_id(uint32_t id);
#endif

    Instruction* op();

public:
    blocktype block_type;

    bool uniq;
    uint32_t uniq_id;

    bool tlf;

    std::list<Instruction*> insts;
};

class SpecialBlock : public Block {
public:
    SpecialBlock() : Block(new Instruction()) {
        Instruction* instr = this->insts.front();

        instr->pc = 0;
    }

    std::string name();

public:
    bool mergeable;
};

#endif /* !BLOCK_HH_ */
