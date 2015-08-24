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
# include "../processors/cpu_functions.hh"
# include "block.hh"
# include "link.hh"

template <typename CPU>
class Graph {
public:
    Graph(std::string filename);
    virtual ~Graph();

    void generate_graph();

private:
    void _cutfunction(Block<CPU>* func);

private:
    FileReader _file;

    LinkMgr<CPU> _link_mgr;

    SpecialBlock<CPU>* _begin;
    SpecialBlock<CPU>* _end;

    std::map<typename cpu_traits<CPU>::AddrType, std::list<Block<CPU>*>> _blocks;
    std::stack<std::pair<Block<CPU>*, typename cpu_traits<CPU>::AddrType>> _backtrace;
};

# include "graph.hxx"

#endif /* !JARBOCUL_GRAPH_GRAPH_HH_ */
