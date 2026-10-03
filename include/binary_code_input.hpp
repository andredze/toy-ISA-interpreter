#pragma once

//————————————————————————————————————————————————————————————————————————————————

#include <string_view>
#include <filesystem>
#include <cstdint>

//————————————————————————————————————————————————————————————————————————————————

namespace binary_files_io
{

//--------------------------------------------------------------------------------

constexpr std::size_t BINCODE_BUFFER_INIT_CAPACITY = 256;

class BinaryCode
{
protected:
    uint8_t* buffer_;
    size_t   capacity_;

public:
    BinaryCode () : 
        buffer_(nullptr), 
        capacity_(0)
    {}

    ~BinaryCode ()
    {
        if (buffer_ != nullptr) {
            delete [] buffer_;
        }
    }

    bool ReadFile (const std::filesystem::path& file_name);
}; // class BinaryCode

//--------------------------------------------------------------------------------

}; // namespace binary_files_io

//————————————————————————————————————————————————————————————————————————————————