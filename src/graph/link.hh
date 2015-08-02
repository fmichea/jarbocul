#pragma once
#ifndef LINK_HH_
# define LINK_HH_

# include <iostream>
# include <list>
# include <map>
# include <set>
# include <string>

# include "block.hh"

typedef enum linktype_e {
    LINKTYPE_NORMAL         = 0x0,
    LINKTYPE_TAKEN          = 0x1,
    LINKTYPE_CALL_TAKEN     = 0x2,
    LINKTYPE_NOT_TAKEN      = 0x3,
    LINKTYPE_RET_MISS       = 0x4,
} linktype;

std::string linktype2str(linktype t);

class Link {
public:
    Link(Block* from, Block* to);
    virtual ~Link() {}

public:
    Block* from;
    Block* to;

    linktype link_type;
};


class LinkMgr {
public:
    LinkMgr();
    virtual ~LinkMgr();

    Link* find_link(Block* from, Block* to, bool do_link=false);

    void do_link(Link* link);
    void do_unlink(Link* link);

    bool accepts_merge_bottom(Block* block);
    bool accepts_merge_top(Block* block);

    std::set<Link*> get_all_links_to_block(Block* block);
    std::set<Link*> get_all_links_from_block(Block* block);

private:
    typedef std::pair<BlockId, BlockId> BlocksToLinkMapKey;
    typedef std::map<BlocksToLinkMapKey, Link*> BlocksToLinkMap;

    typedef std::set<Link*> BlockToLinksIdxValue;
    typedef std::map<BlockId, BlockToLinksIdxValue> BlockToLinksIdx;

private:
    void _add_link_to_idx(BlockToLinksIdx& idx, Block* block, Link* link);
    void _del_link_from_idx(BlockToLinksIdx& idx, Block* block, Link* link);
    bool _idx_contains_one_link_for_block(BlockToLinksIdx& idx, Block* block);

private:
    BlocksToLinkMap _links;

    /* For faster access, index to the list of links from a source and from a
    ** destination.
    **   - _link_sources_idx: block -> all links going to it
    **   - _link_destinations_idx: block -> all links to blocks it goes to */
    BlockToLinksIdx _link_sources_idx;
    BlockToLinksIdx _link_destinations_idx;
};

#endif /* !LINK_HH_ */
