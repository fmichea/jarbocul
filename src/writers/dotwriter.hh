#pragma once
#ifndef JARBOCUL_WRITERS_DOTWRITER_HH_
# define JARBOCUL_WRITERS_DOTWRITER_HH_

# include <algorithm>
# include <boost/algorithm/string.hpp>

# include "writer.hh"

template <typename CPU>
class DotWriter : public Writer<CPU> {
public:
    DotWriter();

protected:
    void _output_function(Block<CPU>* function, LinkMgr<CPU>& link_mgr);
};

# include "dotwriter.hxx"

#endif /* !JARBOCUL_WRITERS_DOTWRITER_HH_ */
