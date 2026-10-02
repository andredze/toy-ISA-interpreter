#include "binary_code_input.hpp"
#include "error_handle.hpp"
#include <istream>
#include <fstream>
#include <cassert>
#include <filesystem>

//————————————————————————————————————————————————————————————————————————————————

namespace binary_files_io
{

using error_handle::PrintError;

//--------------------------------------------------------------------------------

static std::streamsize GetFileSize (std::ifstream& input_file)
{
    assert (input_file.is_open ());

    input_file.seekg(0, std::ios::end);

    std::streamsize input_file_size = input_file.tellg();

    input_file.seekg(0, std::ios::beg);

    return input_file_size;
}

//--------------------------------------------------------------------------------

bool BinaryCode::ReadFile (const std::filesystem::path& file_name)
{
    std::ifstream input_file (file_name, std::ios::binary);

    if (!input_file.is_open ()) {
        PrintError ("Failed to open file {}", file_name.string ());
        return false;
    }

    std::streamsize input_file_size = GetFileSize (input_file);

    if (input_file_size == -1) {
        PrintError ("Failed to read file size for {}", file_name.string ());
        return false;
    }

    buffer_ = new char[input_file_size];

    capacity_ = static_cast<std::size_t>(input_file_size);

    if (!input_file.read (buffer_, input_file_size)) {
        PrintError ("Failed to read file {}", file_name.string ());
        return false;
    }

    input_file.close ();

    return true;
}

//--------------------------------------------------------------------------------

char* BinaryCode::GetChunkOfCode (std::size_t chunk_size)
{
    if (cur_pos_ + chunk_size >= capacity_) {
        return NULL;
    }

    if (buffer_ == NULL) {
        PrintError ("Can not reach binary code, you have to read file first");
        return NULL;
    }

    return &buffer_[cur_pos_];
}

//--------------------------------------------------------------------------------

}; // namespace binary_files_io

//————————————————————————————————————————————————————————————————————————————————