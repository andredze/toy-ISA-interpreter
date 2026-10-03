#pragma once

//--------------------------------------------------------------------------------

#include <string_view>
#include <cstdint>

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

constexpr std::string_view kExecutableFileName = "anki_interpreter"; 

using Word = std::uint32_t;
using Byte = std::uint8_t;

constexpr std::size_t kWordSizeInBits = sizeof(Word) * 8;

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------