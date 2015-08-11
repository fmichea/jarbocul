#pragma once
#ifndef GRAPH_HH_
# define GRAPH_HH_

# include <iostream>
# include <fstream>

# include <list>
# include <map>
# include <queue>
# include <set>
# include <stack>
# include <string>

# include <fcntl.h>
# include <string.h>
# include <sys/mman.h>
# include <sys/stat.h>
# include <unistd.h>

# include "../lib/flowtype.hh"
# include "../processors/gb_z80/disassembler.hh"
# include "block.hh"
# include "link.hh"

class FileReader {
public:
    FileReader(std::string filename);
    virtual ~FileReader();

    bool eof();

    const char* readline();

private:
    int _fd;
    char* _data;

    char* _allocated_data;

    static const size_t RW_DATA_SZ = 1024 * 16;
    char _rw_data[RW_DATA_SZ + 1];

    size_t _size;
    size_t _offset;
};

template <typename CPU>
class Graph {
public:
    Graph(std::string filename);
    virtual ~Graph();

    void generate_graph();

private:
    FileReader _file;

    LinkMgr _link_mgr;

    SpecialBlock<CPU>* _begin;
    SpecialBlock<CPU>* _end;

    std::map<cpu_traits<CPU>::Addr, std::list<Block<CPU>*>> _blocks;
};

#endif /* !GRAPH_HH_ */
