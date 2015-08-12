#include "link.hh"


std::string linktype2str(linktype t) {
    switch (t) {
    case LINKTYPE_NORMAL: return "black";
    case LINKTYPE_TAKEN: return "green";
    case LINKTYPE_CALL_TAKEN: return "blue";
    case LINKTYPE_NOT_TAKEN: return "red";
    case LINKTYPE_RET_MISS: return "brown";
    };
}


template <typename CPU>
Link<CPU>::Link<CPU>(Block<CPU>* from, Block<CPU>* to)
    : from (from)
    , to (to)
    , link_type (LINKTYPE_NORMAL)
{}

template <typename CPU>
LinkMgr<CPU>::LinkMgr<CPU>()
{}

template <typename CPU>
LinkMgr<CPU>::~LinkMgr<CPU>()
{}

template <typename CPU>
Link<CPU>* LinkMgr<CPU>::find_link(Block<CPU>* from, Block<CPU>* to, bool do_link) {
    Link<CPU>* res = nullptr;

    LinkMgr<CPU>::BlocksToLinkMapKey key(from->id(), to->id());
    LinkMgr<CPU>::BlocksToLinkMap::iterator it = this->_links.find(key);

    if (it != this->_links.end()) {
        res = it->second;
    }

    if (res == nullptr) {
        res = new Link<CPU>(from, to);
    }

    if (do_link) {
        this->do_link(res);
    }

    return res;
}

template <typename CPU>
void LinkMgr<CPU>::do_link(Link<CPU>* link) {
    if (link == nullptr)
        return;

    LinkMgr<CPU>::BlocksToLinkMapKey key(link->from->id(), link->to->id());

    this->_links[key] = link;
    this->_add_link_to_idx(this->_link_sources_idx, link->to, link);
    this->_add_link_to_idx(this->_link_destinations_idx, link->from, link);
}

template <typename CPU>
void LinkMgr<CPU>::do_unlink(Link<CPU>* link) {
    if (link == nullptr)
        return;

    LinkMgr<CPU>::BlocksToLinkMapKey key(link->from->id(), link->to->id());

    this->_links.erase(key);
    this->_del_link_from_idx(this->_link_sources_idx, link->to, link);
    this->_del_link_from_idx(this->_link_destinations_idx, link->from, link);
    delete link;
}

template <typename CPU>
void LinkMgr<CPU>::_add_link_to_idx(LinkMgr<CPU>::BlockToLinksIdx& idx,
                                    Block<CPU>* block,
                                    Link<CPU>* link)
{
    idx[block->id()].insert(link);
}

template <typename CPU>
void LinkMgr<CPU>::_del_link_from_idx(BlockToLinksIdx<CPU>& idx,
                                      Block<CPU>* block,
                                      Link<CPU>* link)
{
    LinkMgr<CPU>::BlockToLinksIdx::iterator it = idx.find(block->id());

    if (it != idx.end()) {
        it->second.erase(link);
    }
}

template <typename CPU>
bool LinkMgr<CPU>::accepts_merge_bottom(Block<CPU>* block) {
    return this->_idx_contains_one_link_for_block(
        this->_link_destinations_idx,
        block
    );
}

template <typename CPU>
bool LinkMgr<CPU>::accepts_merge_top(Block<CPU>* block) {
    return this->_idx_contains_one_link_for_block(
        this->_link_sources_idx,
        block
    );
}

template <typename CPU>
bool LinkMgr<CPU>::_idx_contains_one_link_for_block(
    LinkMgr<CPU>::BlockToLinksIdx& idx,
    Block<CPU>* block)
{
    LinkMgr<CPU>::BlockToLinksIdx::iterator it = idx.find(block->id());

    //assert(it != idx.end());
    if (it == idx.end()) {
        // This can happen if an interrupt jumps on another interrupt for
        // instance.
        return false;
    }
    return it->second.size() == 1;
}

template <typename CPU>
 LinkMgr::get_all_links_to_block(Block* block) {
    return this->_link_sources_idx[block->id()];
}

std::set<Link*> LinkMgr::get_all_links_from_block(Block* block) {
    return this->_link_destinations_idx[block->id()];
}
