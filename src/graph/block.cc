#include "block.hh"

std::string blocktype2str(blocktype t) {
    switch (t) {
    case BLOCKTYPE_INT: return "int";
    case BLOCKTYPE_LOC: return "loc";
    case BLOCKTYPE_SUB: return "sub";
    };
}

/* BlockId Block::id() {
    Instruction* inst = this->insts.front();
    BlockId res = 0;

    res |= ((BlockId) inst->pc) << 48;
    res |= ((BlockId) inst->opcode) << 40;
    res |= ((BlockId) inst->data[0]) << 32;
    res |= ((BlockId) inst->data[1]) << 24;
    return res;
} */
