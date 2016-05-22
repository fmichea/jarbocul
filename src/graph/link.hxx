#ifndef JARBOCUL_GRAPH_LINK_HXX_
# define JARBOCUL_GRAPH_LINK_HXX_

#include "link.hh"

template <typename CPU>
Link<CPU>::Link(Block<CPU>* from, Block<CPU>* to)
    : _from (from)
    , _to (to)
    , _link_type (LINKTYPE_NORMAL)
{}

template <typename CPU>
Link<CPU>::~Link()
{}

template <typename CPU>
Block<CPU>* Link<CPU>::from() const {
    return this->_from;
}

template <typename CPU>
Block<CPU>* Link<CPU>::to() const {
    return this->_to;
}

template <typename CPU>
linktype Link<CPU>::link_type() const {
    return this->_link_type;
}

template <typename CPU>
void Link<CPU>::set_link_type(linktype link_type) {
    this->_link_type = link_type;
}

template <typename CPU>
LinkMgr<CPU>::LinkMgr()
{}

template <typename CPU>
LinkMgr<CPU>::~LinkMgr()
{}

template <typename CPU>
Link<CPU>* LinkMgr<CPU>::find_link(Block<CPU>* from, Block<CPU>* to, bool do_link) {
    Link<CPU>* res = nullptr;

    typename LinkMgr<CPU>::BlocksToLinkMapKey key(from->id(), to->id());
    typename LinkMgr<CPU>::BlocksToLinkMap::iterator it = this->_links.find(key);

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

    LinkMgr<CPU>::BlocksToLinkMapKey key(link->from()->id(), link->to()->id());

    if (this->_links.find(key) != this->_links.end()) {
        return;
    }

    this->_links[key] = link;
    this->_add_link_to_idx(this->_link_sources_idx, link->to(), link);
    this->_add_link_to_idx(this->_link_destinations_idx, link->from(), link);
}

template <typename CPU>
void LinkMgr<CPU>::do_unlink(Link<CPU>* link) {
    if (link == nullptr)
        return;

    LinkMgr<CPU>::BlocksToLinkMapKey key(link->from()->id(), link->to()->id());

    this->_links.erase(key);
    this->_del_link_from_idx(this->_link_sources_idx, link->to(), link);
    this->_del_link_from_idx(this->_link_destinations_idx, link->from(), link);
    delete link;
}

template <typename CPU>
void LinkMgr<CPU>::_add_link_to_idx(typename LinkMgr<CPU>::BlockToLinksIdx& idx,
                                    Block<CPU>* block,
                                    Link<CPU>* link)
{
    idx[block->id()].insert(link);
}

template <typename CPU>
void LinkMgr<CPU>::_del_link_from_idx(typename LinkMgr<CPU>::BlockToLinksIdx& idx,
                                      Block<CPU>* block,
                                      Link<CPU>* link)
{
    typename LinkMgr<CPU>::BlockToLinksIdx::iterator it = idx.find(block->id());

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
    typename LinkMgr<CPU>::BlockToLinksIdx& idx,
    Block<CPU>* block)
{
    typename LinkMgr<CPU>::BlockToLinksIdx::iterator it = idx.find(block->id());

    if (it == idx.end()) {
        // This can happen if an interrupt jumps on another interrupt for
        // instance.
        return false;
    }
    return it->second.size() == 1;
}

template <typename CPU>
typename LinkMgr<CPU>::BlockToLinksIdxValue
LinkMgr<CPU>::get_all_links_to_block(Block<CPU>* block) {
    return this->_link_sources_idx[block->id()];
}

template <typename CPU>
typename LinkMgr<CPU>::BlockToLinksIdxValue
LinkMgr<CPU>::get_all_links_from_block(Block<CPU>* block) {
    return this->_link_destinations_idx[block->id()];
}

template <typename CPU>
bool LinkMgr<CPU>::triggering_link_found(Block<CPU>* block) const {
    return this->_tlf_for_block.count(block->id()) == 1;
}

template <typename CPU>
void LinkMgr<CPU>::set_triggering_link_found(Block<CPU>* block) {
    this->_tlf_for_block.insert(block->id());
}

#endif /* !JARBOCUL_GRAPH_LINK_HXX_ */
