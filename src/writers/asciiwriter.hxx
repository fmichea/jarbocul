#pragma once
#ifndef JARBOCUL_WRITERS_ASCIIWRITER_HXX_
# define JARBOCUL_WRITERS_ASCIIWRITER_HXX_

# include "asciiwriter.hh"

template <typename CPU>
AsciiWriter<CPU>::AsciiWriter()
    : Writer<CPU>()
    , _ext ("txt")
{}

template <typename CPU>
void AsciiWriter<CPU>::_generate_function(Block<CPU>& function) {
}

#endif /* !JARBOCUL_WRITERS_ASCIIWRITER_HXX_ */
