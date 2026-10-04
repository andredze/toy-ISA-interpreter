#pragma once

//--------------------------------------------------------------------------------

#include <cstdint>
#include "config.hpp"
#include "binary_code_input.hpp"
#include "error_handle.hpp"
#include "decode.hpp"

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

using Register = std::uint32_t;

enum class GeneralPurposeRegisterName
{
    X0, // return value
    X1, // arg0
    X2, // arg1
    X3, // arg2
    X4, // arg3
    X5, // arg4
    X6, // arg5
    X7, // arg6
    X8, // syscall number
    X9,  X10, X11, X12, 
    X13, X14, X15, X16, 
    X17, X18, X19, X20,
    X21, X22, X23, X24,
    X25, X26, X27, X28,
    X29, X30,
    X32, // LR (link register, for return address)
};

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

    Instruction Decode (Word encoding);
}; // class CPUState

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------