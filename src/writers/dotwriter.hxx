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
bool link_sorter(const Link<CPU>* a, const Link<CPU>* b) {
    return a->to()->pc() < b->to()->pc();
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

        std::set<Link<CPU>*> links_set = link_mgr.get_all_links_from_block(
            func_block);

        std::vector<Link<CPU>*> links(links_set.size());
        std::copy(links_set.begin(), links_set.end(), links.begin());

        typedef bool (*comparer_t)(const Link<CPU>*, const Link<CPU>*);
        comparer_t link_sorter_loc = link_sorter;

        std::sort(links.begin(), links.end(), link_sorter_loc);

        for (Link<CPU>* link : links) {
            if (link->from()->pc() == 0xFFBC) {
                std::cout << "link:" << link << std::endl;
            }

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
