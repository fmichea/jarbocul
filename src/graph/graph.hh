#pragma once
#ifndef GRAPH_HH_
# define GRAPH_HH_

# include <iostream>
# include <fstream>

# include <list>
# include <map>
# include <stack>
# include <string>

# include "block.hh"
# include "link.hh"

class Graph {
public:
    Graph(std::string filename);
    virtual ~Graph();

    void generate_graph();

private:
    std::ifstream _file;

    LinkMgr _link_mgr;

    SpecialBlock* _begin;
    SpecialBlock* _end;
};

#endif /* !GRAPH_HH_ */
