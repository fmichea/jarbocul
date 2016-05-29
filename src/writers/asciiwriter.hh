#pragma once
#ifndef JARBOCUL_WRITERS_ASCIIWRITER_HH_
# define JARBOCUL_WRITERS_ASCIIWRITER_HH_

# include "writer.hh"

template <typename CPU>
class AsciiWriter : public Writer<CPU> {
public:
    AsciiWriter();

protected:
    void _generate_function(Block<CPU>& function);
};

# include "asciiwriter.hxx"

#endif /* !JARBOCUL_WRITERS_ASCIIWRITER_HH_ */
