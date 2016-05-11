#ifndef JARBOCUL_WRITERS_DOTWRITER_HXX_
# define JARBOCUL_WRITERS_DOTWRITER_HXX_

# include "dotwriter.hh"

template <typename CPU>
DotWriter<CPU>::DotWriter()
    : Writer<CPU>()
{
    this->_ext = "dot";
}

template <typename CPU>
void DotWriter<CPU>::_output_function(Block<CPU>* function,
                                      LinkMgr<CPU>& link_mgr)
{
    std::set<BlockId> done;
    std::queue<Block<CPU>*> todo;

    std::ofstream out(this->_output_filename(function->name()));

    out << "digraph " << function->name() << " {\n";
    out << "\tsplines = true;\n";
    out << "\tnode [ shape = box, fontname = \"Monospace\" ];\n\n";

    todo.push(function);
    while (!todo.empty()) {
        Block<CPU>* func_block = todo.front();
        todo.pop();

        if (done.count(func_block->id()) != 0) {
            continue;
        }
        done.insert(func_block->id());

        std::ostringstream label_stream;
        label_stream << *func_block;
        std::string label = boost::replace_all_copy(label_stream.str(), "\n", "\\l");

        out << "\t" << func_block->name() << " [ label = \"" << label << "\" ];\n";

        for (Link<CPU>* link : link_mgr.get_all_links_from_block(func_block)) {
            out << "\t"
               << link->from()->name()
               << " -> "
               << link->to()->name()
               << " [ color = " << linktype2str(link->link_type())
               << ", tailport = s, headport = n ];\n";
            todo.push(link->to());
        }
        out << "\n";
    }

    out << "}\n";
    out.close();
}

#endif /* !JARBOCUL_WRITERS_DOTWRITER_HXX_ */
