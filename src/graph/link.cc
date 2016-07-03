# include "graph/link.hh"

std::string linktype2str(linktype t) {
    switch (t) {
    case LINKTYPE_NORMAL: return "black";
    case LINKTYPE_TAKEN: return "green";
    case LINKTYPE_CALL_TAKEN: return "blue";
    case LINKTYPE_NOT_TAKEN: return "red";
    case LINKTYPE_RET_MISS: return "brown";
    };
}
