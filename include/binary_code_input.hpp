#pragma once

//————————————————————————————————————————————————————————————————————————————————

#include <string_view>
#include <filesystem>
#include <cstdint>

//————————————————————————————————————————————————————————————————————————————————

namespace binary_files_io
{
    constexpr std::size_t BINCODE_BUFFER_INIT_CAPACITY = 256;

class BinaryCode
{
private:
    char*  buffer_;
    size_t cur_pos_;
    size_t capacity_;

public:
    BinaryCode () : 
        buffer_(nullptr), 
        cur_pos_(0),
        capacity_(0)
    {}

    ~BinaryCode ()
    {
        if (buffer_ != nullptr) {
            delete [] buffer_;
        }
    }

    bool ReadFile (const std::filesystem::path& file_name);

    char* GetChunkOfCode (std::size_t chunk_size);
}; // class BinaryCode

}; // namespace binary_files_io

//————————————————————————————————————————————————————————————————————————————————