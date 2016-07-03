#pragma once
#ifndef JARBOCUL_GRAPH_GRAPH_HH_
# define JARBOCUL_GRAPH_GRAPH_HH_

# include <fstream>
# include <iostream>
# include <list>
# include <map>
# include <queue>
# include <set>
# include <string>

# include "graph/backtrace.hh"
# include "graph/block.hh"
# include "graph/link.hh"
# include "lib/file_reader.hh"
# include "lib/flowtype.hh"
# include "lib/set_sorter.hh"
# include "processors/cpu_functions.hh"

template <typename CPU>
class Graph {
public:
    Graph(std::string filename);
    virtual ~Graph();

    std::list<Block<CPU>*> functions();
    LinkMgr<CPU>& link_mgr();

    void generate_graph();

private:
    typedef typename cpu_traits<CPU>::AddrType AddrType;

    typedef Block<CPU> BlockType;
    typedef Link<CPU> LinkType;
    typedef Instruction<CPU> InstructionType;

private:
    void _cutfunction(BlockType* func);
    void _merge_blocks(BlockType* block);

private:
    FileReader _file;

    LinkMgr<CPU> _link_mgr;

    SpecialLabelBlock<CPU>* _begin;
    SpecialLabelBlock<CPU>* _end;

    std::map<AddrType, std::list<BlockType*>> _blocks;

    Backtrace<CPU> _backtrace;

    std::list<BlockType*> _functions;
};

# include "graph/graph.hxx"

#endif /* !JARBOCUL_GRAPH_GRAPH_HH_ */
