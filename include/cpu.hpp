#pragma once

//--------------------------------------------------------------------------------

#include <cstdint>

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

constexpr std::size_t kGeneralPurposeRegistersNumber = 32;

using Register = std::uint32_t;

class CPUState
{
    Register general_purpose_regs_[kGeneralPurposeRegistersNumber];
    Register program_counter_;

}; // class CPUState

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------