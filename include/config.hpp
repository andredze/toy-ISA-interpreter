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

constexpr int kOptimizationLevel = 2;
constexpr int kBBTranslationThreshold = 1;

constexpr std::string_view kDotDir = "dot";
constexpr std::string_view kSvgDir = "svg";
constexpr std::string_view kDotFileName = "bt";

constexpr std::size_t kWordSizeInBits = sizeof(Word) * 8;

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------
