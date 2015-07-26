#pragma once
#ifndef LINK_HH_
# define LINK_HH_

# include <string>
# include <map>
# include <list>
# include <set>

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
    Link(const Block* from, const Block* to);
    virtual ~Link() {}

#if 0
    void set_link_type(linktype t);

    void do_link(int n = 1);
    void do_unlink();

    void unlink_all();
#endif

public:
    const Block* from;
    const Block* to;

    linktype link_type;

#if 0
    uint64_t _count;
#endif
};


class LinkMgr {
public:
    LinkMgr();
    virtual ~LinkMgr();

    Link* find_link(const Block* from, const Block* to, bool do_link=false);

    void do_link(Link* link);
    void do_unlink(Link* link);

    bool accepts_merge_bottom(const Block* block);
    bool accepts_merge_top(const Block* block);

    std::set<Link*> get_all_links_to_block(const Block* block);
    std::set<Link*> get_all_links_from_block(const Block* block);

private:
    typedef std::pair<BlockId, BlockId> BlocksToLinkMapKey;
    typedef std::map<BlocksToLinkMapKey, Link*> BlocksToLinkMap;

    typedef std::set<Link*> BlockToLinksIdxValue;
    typedef std::map<BlockId, BlockToLinksIdxValue> BlockToLinksIdx;

private:
    void _add_link_to_idx(BlockToLinksIdx& idx, const Block* block, Link* link);
    void _del_link_from_idx(BlockToLinksIdx& idx, const Block* block, Link* link);
    bool _idx_contains_one_link_for_block(BlockToLinksIdx& idx, const Block* block);

private:
    BlocksToLinkMap _links;

    /* For faster access, index to the list of links from a source and from a
    ** destination.
    **   - _link_sources_idx: block -> all blocks going to it
    **   - _link_destinations_idx: block -> all the blocks it goes to */
    BlockToLinksIdx _link_sources_idx;
    BlockToLinksIdx _link_destinations_idx;
};

#endif /* !LINK_HH_ */
