#pragma once
#ifndef JARBOCUL_GRAPH_LINK_HH_
# define JARBOCUL_GRAPH_LINK_HH_

# include <iostream>
# include <list>
# include <map>
# include <set>
# include <string>

# include "graph/block.hh"

typedef enum linktype_e {
    LINKTYPE_NORMAL         = 0x0,
    LINKTYPE_TAKEN          = 0x1,
    LINKTYPE_CALL_TAKEN     = 0x2,
    LINKTYPE_NOT_TAKEN      = 0x3,
    LINKTYPE_RET_MISS       = 0x4,
} linktype;

std::string linktype2str(linktype t);

template <typename CPU>
class Link {
public:
    Link(Block<CPU>* from, Block<CPU>* to);
    virtual ~Link();

    Block<CPU>* from() const;
    Block<CPU>* to() const;

    linktype link_type() const;
    void set_link_type(linktype link_type);

private:
    Block<CPU>* _from;
    Block<CPU>* _to;

    linktype _link_type;
};

template <typename CPU>
class LinkMgr {
public:
    typedef std::set<Link<CPU>*> BlockToLinksIdxValue;

public:
    LinkMgr();
    virtual ~LinkMgr();

    Link<CPU>* find_link(Block<CPU>* from, Block<CPU>* to, bool do_link=false);

    void do_link(Link<CPU>* link);
    void do_unlink(Link<CPU>* link);

    bool accepts_merge_bottom(Block<CPU>* block);
    bool accepts_merge_top(Block<CPU>* block);

    BlockToLinksIdxValue get_all_links_to_block(Block<CPU>* block);
    BlockToLinksIdxValue get_all_links_from_block(Block<CPU>* block);

    bool triggering_link_found(Block<CPU>* block) const;
    void set_triggering_link_found(Block<CPU>* block);

private:
    typedef std::pair<BlockId, BlockId> BlocksToLinkMapKey;
    typedef std::map<BlocksToLinkMapKey, Link<CPU>*> BlocksToLinkMap;
    typedef std::map<BlockId, BlockToLinksIdxValue> BlockToLinksIdx;

private:
    void _add_link_to_idx(BlockToLinksIdx& idx, Block<CPU>* block, Link<CPU>* link);
    void _del_link_from_idx(BlockToLinksIdx& idx, Block<CPU>* block, Link<CPU>* link);
    bool _idx_contains_one_link_for_block(BlockToLinksIdx& idx, Block<CPU>* block);

private:
    BlocksToLinkMap _links;

    /* For faster access, index to the list of links from a source and from a
    ** destination.
    **   - _link_sources_idx: block -> all links going to it
    **   - _link_destinations_idx: block -> all links to blocks it goes to */
    BlockToLinksIdx _link_sources_idx;
    BlockToLinksIdx _link_destinations_idx;

    /* This is used to know if we are on the first triggering link for that
    ** block. It contains all block ids where we already found a triggering
    ** link. */
    std::set<BlockId> _tlf_for_block;

};

# include "graph/link.hxx"

#endif /* !JARBOCUL_GRAPH_LINK_HH_ */
