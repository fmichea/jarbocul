#pragma once
#ifndef JARBOCUL_GRAPH_BACKTRACE_HH_
# define JARBOCUL_GRAPH_BACKTRACE_HH_

# include <stack>

# include "graph/block.hh"

template <typename CPU>
class BacktraceItem {
public:
    BacktraceItem(Block<CPU>* last_block, size_t size);

    Block<CPU>* last_block();
    size_t size();

private:
    Block<CPU>* _last_block;
    size_t _size;
};

template <typename CPU>
class Backtrace {
public:
    Backtrace();
    virtual ~Backtrace();

    bool empty();
    void push(Block<CPU>* last_block, size_t size);
    BacktraceItem<CPU>* top();
    void pop();

private:
    std::stack<BacktraceItem<CPU>*> _items;
};

# include "graph/backtrace.hxx"

#endif /* !JARBOCUL_GRAPH_BACKTRACE_HH_ */
