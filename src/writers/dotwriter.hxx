#pragma once
#ifndef JARBOCUL_WRITERS_DOTWRITER_HXX_
# define JARBOCUL_WRITERS_DOTWRITER_HXX_

# include "writers/dotwriter.hh"

template <typename CPU>
DotWriter<CPU>::DotWriter()
    : Writer<CPU>()
{
    this->_ext = "dot";
}

template <typename CPU>
bool link_sorter_tos(Link<CPU>* a, Link<CPU>* b) {
    if (a->to()->pc() == b->to()->pc()) {
        return a->to()->uniq_id() < b->to()->uniq_id();
    }
    return a->to()->pc() < b->to()->pc();
}

template <typename CPU>
bool block_sorter(Block<CPU>* a, Block<CPU>* b) {
    if (a->pc() == b->pc()) {
        return a->uniq_id() < b->uniq_id();
    }
    return a->pc() < b->pc();
}

template <typename CPU>
void DotWriter<CPU>::_output_function(Block<CPU>* function,
                                      LinkMgr<CPU>& link_mgr)
{
    typedef Block<CPU>* BlockPtr;
    typedef Link<CPU>* LinkPtr;

    std::set<BlockPtr>   all_function_blocks_set;
    std::queue<BlockPtr> todo;

    todo.push(function);
    while (!todo.empty()) {
        BlockPtr func_block = todo.front();
        todo.pop();

        if (all_function_blocks_set.count(func_block) != 0) {
            continue;
        }
        all_function_blocks_set.insert(func_block);

        for (LinkPtr link : link_mgr.get_all_links_from_block(func_block)) {
            todo.push(link->to());
        }
    }

    std::vector<BlockPtr> all_function_blocks = set_sorter(
        all_function_blocks_set,
        block_sorter<CPU>
    );

    std::ofstream out(this->_output_filename(function->name()));

    out << "digraph " << function->name() << " {\n";
    out << "\tsplines = true;\n";
    out << "\tnode [ shape = box, fontname = \"Monospace\" ];\n\n";

    for (BlockPtr func_block : all_function_blocks) {
        std::ostringstream label_stream;
        label_stream << *func_block;
        std::string label = boost::replace_all_copy(label_stream.str(), "\n", "\\l");

        out << "\t" << func_block->uniq_name() << " [ label = \"" << label << "\" ];\n";

        std::vector<LinkPtr> links = set_sorter(
            link_mgr.get_all_links_from_block(func_block),
            link_sorter_tos<CPU>
        );

        for (LinkPtr link : links) {
            out << "\t"
               << link->from()->uniq_name()
               << " -> "
               << link->to()->uniq_name()
               << " [ color = " << linktype2str(link->link_type())
               << ", tailport = s, headport = n ];\n";
        }
        out << "\n";
    }

    out << "}\n";
    out.close();
}

#endif /* !JARBOCUL_WRITERS_DOTWRITER_HXX_ */
