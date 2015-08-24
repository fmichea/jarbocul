#pragma once
#ifndef JARBOCUL_LIB_FILE_READER_HH_
# define JARBOCUL_LIB_FILE_READER_HH_

# include <string>

# include <fcntl.h>
# include <string.h>
# include <sys/mman.h>
# include <sys/stat.h>
# include <unistd.h>

class FileReader {
public:
    FileReader(std::string filename);
    virtual ~FileReader();

    const char* readline();

    bool eof();

private:
    int _fd;
    char* _data;

    char* _allocated_data;

    static const size_t RW_DATA_SZ = 1024 * 16;
    char _rw_data[RW_DATA_SZ + 1];

    size_t _size;
    size_t _offset;
};

#endif /* !JARBOCUL_LIB_FILE_READER_HH_ */
