#pragma once
#ifndef JARBOCUL_WRITERS_WRITER_HXX_
# define JARBOCUL_WRITERS_WRITER_HXX_

# include "writer.hh"

template <typename CPU>
Writer<CPU>::Writer()
    : _ext ("txt")
    , _output_dir ("result")
{}

template <typename CPU>
void Writer<CPU>::set_output_dir(std::string output_dir) {
    this->_output_dir = output_dir;
}

template <typename CPU>
std::string Writer<CPU>::_output_filename(std::string filename) {
    return this->_output_filename(filename, this->_ext);
}

template <typename CPU>
std::string Writer<CPU>::_output_filename(std::string fname, std::string ext) {
    std::ostringstream filename;

    filename << this->_output_dir << "/" << fname << "." << ext;
    return filename.str();
}

template <typename CPU>
void Writer<CPU>::output(Graph<CPU>& g) {
    for (Block<CPU>* function : g.functions()) {
        this->_output_function(function, g.link_mgr());
    }
}

#endif /* !JARBOCUL_WRITERS_WRITER_HXX_ */
