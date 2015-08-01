#include "block.hh"

# define BLOCKNAME_SZ (sizeof (uint16_t) * 2)

std::string blocktype2str(blocktype t) {
    switch (t) {
    case BLOCKTYPE_INT: return "int";
    case BLOCKTYPE_LOC: return "loc";
    case BLOCKTYPE_SUB: return "sub";
    };
}

Block::Block(Instruction* inst)
    : pc (inst->pc)
    , block_type (BLOCKTYPE_LOC)
    , uniq (true)
    , uniq_id (0)
    , tlf (false)
    , insts ()
    , within ()
    , mergeable (true)
{
    this->insts.push_back(inst);
}

Block::~Block()
{}

std::string Block::name() {
    std::ostringstream result;
    std::ostringstream addr;

    addr << std::setfill('0') << std::setw(4) << std::hex << std::uppercase
         << this->pc;

    result << blocktype2str(this->block_type) << this->_sep() << addr.str();
    if (!this->uniq) {
        result << "-" << this->uniq_id;
    }
    return result.str();
}

BlockId Block::id() {
    Instruction* inst = this->insts.front();
    BlockId res = 0;

    res |= ((BlockId) inst->pc) << 48;
    res |= ((BlockId) inst->opcode) << 40;
    res |= ((BlockId) inst->data[0]) << 32;
    res |= ((BlockId) inst->data[1]) << 24;
    return res;
}

Instruction* Block::op() {
    assert(this->insts.size() == 1);
    return this->insts.front();
}

void Block::merge(Block* other) {
    assert(this != other);
    for (Instruction* inst : other->insts) {
        this->insts.push_back(inst);
    }
}

#if 0
void Block::merge(Block* other) {
    
}
#endif



std::string SpecialBlock::name() {
    return Block::name() + "S";
}
