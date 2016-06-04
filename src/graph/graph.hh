#pragma once
#ifndef JARBOCUL_GRAPH_GRAPH_HH_
# define JARBOCUL_GRAPH_GRAPH_HH_

# include <iostream>
# include <fstream>

# include <list>
# include <map>
# include <queue>
# include <set>
# include <stack>
# include <string>

# include "../lib/file_reader.hh"
# include "../lib/flowtype.hh"
# include "../lib/set_sorter.hh"
# include "../processors/cpu_functions.hh"
# include "block.hh"
# include "link.hh"

template <typename CPU>
class Graph {
public:
    Graph(std::string filename);
    virtual ~Graph();

    std::list<Block<CPU>*> functions() { return this->_functions; }

    LinkMgr<CPU>& link_mgr() { return this->_link_mgr; }

    void generate_graph();

private:
    void _cutfunction(Block<CPU>* func);
    void _merge_blocks(Block<CPU>* block);

private:
    FileReader _file;

    LinkMgr<CPU> _link_mgr;

    SpecialLabelBlock<CPU>* _begin;
    SpecialLabelBlock<CPU>* _end;

    std::map<typename cpu_traits<CPU>::AddrType, std::list<Block<CPU>*>> _blocks;
    std::stack<std::pair<Block<CPU>*, typename cpu_traits<CPU>::AddrType>> _backtrace;

    std::list<Block<CPU>*> _functions;
};

# include "graph.hxx"

#endif /* !JARBOCUL_GRAPH_GRAPH_HH_ */
