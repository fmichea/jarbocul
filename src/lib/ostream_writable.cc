#include "ostream_writable.hh"

std::ostream& operator << (std::ostream& os, const OStreamWritable& osw) {
    osw._ostream_write(os);
    return os;
}
