#ifndef JARBOCUL_WRITERS_WRITER_HH_
# define JARBOCUL_WRITERS_WRITER_HH_

# include <fstream>
# include <iostream>
# include <sstream>
# include <string>

# include "../graph/graph.hh"

template <typename CPU>
class Writer {
public:
    Writer();

    void set_output_dir(std::string output_dir);

    virtual void output(Graph<CPU>& g);

protected:
    virtual void _output_function(Block<CPU>* function, LinkMgr<CPU>& link_mgr) = 0;

protected:
    std::string _output_filename(std::string fname);
    std::string _output_filename(std::string fname, std::string ext);

protected:
    std::string _ext;
    std::string _output_dir;
};

# include "writer.hxx"

#endif /* !JARBOCUL_WRITERS_WRITER_HH_ */
