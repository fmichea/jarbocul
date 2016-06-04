#pragma once
#ifndef JARBOCUL_GRAPH_BACKTRACE_HXX_
# define JARBOCUL_GRAPH_BACKTRACE_HXX_

# include "backtrace.hh"

template <typename CPU>
BacktraceItem<CPU>::BacktraceItem(Block<CPU>* last_block, size_t size)
    : _last_block (last_block)
    , _size (size)
{}

template <typename CPU>
Block<CPU>* BacktraceItem<CPU>::last_block() {
    return this->_last_block;
}

template <typename CPU>
size_t BacktraceItem<CPU>::size() {
    return this->_size;
}

template <typename CPU>
Backtrace<CPU>::Backtrace()
{}

template <typename CPU>
Backtrace<CPU>::~Backtrace() {
    while (this->empty()) {
        this->pop();
    }
}

template <typename CPU>
bool Backtrace<CPU>::empty() {
    return this->_items.empty();
}

template <typename CPU>
void Backtrace<CPU>::push(Block<CPU>* last_block, size_t size) {
    BacktraceItem<CPU>* bt_item = new BacktraceItem<CPU>(last_block, size);
    this->_items.push(bt_item);
}

template <typename CPU>
BacktraceItem<CPU>* Backtrace<CPU>::top() {
    return this->_items.top();
}

template <typename CPU>
void Backtrace<CPU>::pop() {
    BacktraceItem<CPU>* item = this->top();

    delete item;
    this->_items.pop();
}

#endif /* !JARBOCUL_GRAPH_BACKTRACE_HXX_ */
