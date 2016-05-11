#pragma once
#ifndef JARBOCUL_LIB_OSTREAM_WRITABLE_HH_
# define JARBOCUL_LIB_OSTREAM_WRITABLE_HH_

# include <iostream>

class OStreamWritable {
public:
    friend std::ostream& operator << (std::ostream& os, const OStreamWritable& osw);

protected:
    virtual void _ostream_write(std::ostream& os) const = 0;
};

#endif /* !JARBOCUL_LIB_OSTREAM_WRITABLE_HH_ */
