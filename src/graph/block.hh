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

    virtual std::string name() ;
    virtual BlockId id() ;

    void merge(Block* other);

#if 0
    void set_uniq(bool uniq);
    void set_uniq_id(uint32_t id);
#endif

    Instruction* op();

public:
    uint16_t& pc;

    blocktype block_type;

    bool uniq;
    uint32_t uniq_id;

    bool tlf;

    std::list<Instruction*> insts;

    std::list<std::string> within;

    bool mergeable;

    virtual const char* _sep()  { return "_"; };
};

class SpecialBlock : public Block {
public:
    SpecialBlock() : Block(new Instruction()) {
        Instruction* instr = this->insts.front();

        instr->pc = 0;
    }

    std::string name() ;

private:
    const char* _sep()  { return "_1"; };
};

#endif /* !BLOCK_HH_ */
