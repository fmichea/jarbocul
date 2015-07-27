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


Link::Link(Block* from, Block* to)
    : from (from)
    , to (to)
    , link_type (LINKTYPE_NORMAL)
    //, _count (0)
{}

#if 0
void Link::do_link(int n = 0) {
    this->_count += n;
}

void Link::set_link_type(linktype t) {
    this->_type = t;
}
#endif

LinkMgr::LinkMgr()
{}


LinkMgr::~LinkMgr() {
#if 0
    for (Link* link : this->_links) {
        delete link;
    }
    this->_links.clear();
#endif
}

#include <iostream>

Link* LinkMgr::find_link(Block* from, Block* to, bool do_link) {
    Link* res = nullptr;

    LinkMgr::BlocksToLinkMapKey key(from->id(), to->id());
    LinkMgr::BlocksToLinkMap::iterator it = this->_links.find(key);

    if (it != this->_links.end()) {
        res = it->second;
    }

    if (res == nullptr) {
        res = new Link(from, to);
    }

    if (do_link) {
        this->do_link(res);
    }

    return res;
}

void LinkMgr::do_link(Link* link) {
    if (link == nullptr)
        return;

    LinkMgr::BlocksToLinkMapKey key(link->from->id(), link->to->id());

    this->_links[key] = link;
    this->_add_link_to_idx(this->_link_sources_idx, link->to, link);
    this->_add_link_to_idx(this->_link_destinations_idx, link->from, link);
}

void LinkMgr::do_unlink(Link* link) {
    if (link == nullptr)
        return;

    LinkMgr::BlocksToLinkMapKey key(link->from->id(), link->to->id());

    this->_links.erase(key);
    this->_del_link_from_idx(this->_link_sources_idx, link->to, link);
    this->_del_link_from_idx(this->_link_destinations_idx, link->from, link);

    delete link;
}

void LinkMgr::_add_link_to_idx(LinkMgr::BlockToLinksIdx& idx,
                               Block* block,
                               Link* link)
{
    LinkMgr::BlockToLinksIdx::iterator it = idx.find(block->id());

    if (it == idx.end()) {
        idx[block->id()] = LinkMgr::BlockToLinksIdxValue();
    }
    idx[block->id()].insert(link);
}

void LinkMgr::_del_link_from_idx(BlockToLinksIdx& idx,
                                 Block* block,
                                 Link* link)
{
    LinkMgr::BlockToLinksIdx::iterator it = idx.find(block->id());

    if (it != idx.end()) {
        it->second.erase(link);
    }
}

bool LinkMgr::accepts_merge_bottom(Block* block) {
    return this->_idx_contains_one_link_for_block(
        this->_link_destinations_idx,
        block
    );
}

bool LinkMgr::accepts_merge_top(Block* block) {
    return this->_idx_contains_one_link_for_block(
        this->_link_sources_idx,
        block
    );
}

bool LinkMgr::_idx_contains_one_link_for_block(BlockToLinksIdx& idx,
                                               Block* block)
{
    LinkMgr::BlockToLinksIdx::iterator it = idx.find(block->id());

    //assert(it != idx.end());
    if (it == idx.end()) {
        // This can happen if an interrupt jumps on another interrupt for
        // instance.
        return false;
    }
    return it->second.size() == 1;
}

std::set<Link*> LinkMgr::get_all_links_to_block(Block* block) {
    return this->_link_sources_idx[block->id()];
}

std::set<Link*> LinkMgr::get_all_links_from_block(Block* block) {
    return this->_link_destinations_idx[block->id()];
}
