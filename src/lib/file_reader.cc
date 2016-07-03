#include "lib/file_reader.hh"

FileReader::FileReader(std::string filename)
    : _allocated_data (nullptr)
    , _offset (0)
{
    struct stat st;

    this->_fd = open(filename.c_str(), O_RDONLY);

    fstat(this->_fd, &st);
    this->_size = st.st_size;

    void* data = mmap(NULL, this->_size, PROT_READ, MAP_SHARED, this->_fd, 0);
    this->_data = static_cast<char*>(data);
}

FileReader::~FileReader() {
    if (this->_allocated_data != nullptr)
        delete this->_allocated_data;
    munmap(this->_data, this->_size);
    close(this->_fd);
}

bool FileReader::eof() {
    return this->_offset == this->_size;
}

const char* FileReader::readline() {
    std::string result;

    if (this->_allocated_data != nullptr) {
        delete this->_allocated_data;
        this->_allocated_data = nullptr;
    }

    const char* start = this->_data + this->_offset;
    size_t len = this->_size - this->_offset;

    const char* end = static_cast<const char*>(memchr(start, '\n', len));
    if (end != nullptr) {
        len = static_cast<size_t>(end - start);
    }

    char* res = this->_rw_data;
    if (FileReader::RW_DATA_SZ < len) {
        this->_allocated_data = new char[len + 1];
        res = this->_allocated_data;
    }
    memcpy(res, start, len);
    res[len] = 0;

    this->_offset += len + 1;
    return res;
}
