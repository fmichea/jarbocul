#include "graph/block.hh"

std::string blocktype2str(blocktype t) {
    switch (t) {
    case BLOCKTYPE_INT: return "int";
    case BLOCKTYPE_LOC: return "loc";
    case BLOCKTYPE_SUB: return "sub";
    };
}
