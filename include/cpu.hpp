#pragma once

//--------------------------------------------------------------------------------

#include <cstdint>
#include "config.hpp"
#include "binary_code_input.hpp"
#include "error_handle.hpp"

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

using Register = std::uint32_t;

//--------------------------------------------------------------------------------

class BinaryCode : public binary_files_io::BinaryCode
{
public:
    Word GetInstructionEncoding (Register program_counter)
    {
        if (program_counter >= capacity_) {
            std::string message = "Out of bounds: Program counter exceeds code capacity";
            error_handle::PrintError ("{}", message);
            throw std::runtime_error (message);
        }

        return *(Word*)(buffer_ + program_counter);
    }
};

//--------------------------------------------------------------------------------

constexpr std::size_t kGeneralPurposeRegistersNumber = 32;

//--------------------------------------------------------------------------------

class CpuState
{
    Register general_purpose_regs_[kGeneralPurposeRegistersNumber];
    Register program_counter_;

public:
    CpuState () : general_purpose_regs_{}, program_counter_(0) {};

    Word Fetch (BinaryCode& code)
    {
        return code.GetInstructionEncoding (program_counter_);
    }
}; // class CPUState

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------