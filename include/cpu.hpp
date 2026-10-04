#pragma once

//--------------------------------------------------------------------------------

#include <cstdint>
#include <cassert>
#include "config.hpp"
#include "binary_code_input.hpp"
#include "error_handle.hpp"
#include "decode.hpp"

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

class BinaryCode : public binary_files_io::BinaryCode
{
public:
    Word GetInstructionEncoding (RegisterValue program_counter)
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
private:
    RegisterValue general_purpose_regs_[kGeneralPurposeRegistersNumber];
    RegisterValue program_counter_;

    void SetRegValue (GPR reg, RegisterValue value)
    {
        unsigned reg_code = static_cast<unsigned>(reg);

        assert (reg_code < kGeneralPurposeRegistersNumber);

        general_purpose_regs_[reg_code] = value;
    }

    RegisterValue GetRegValue (GPR reg) const
    {
        unsigned reg_code = static_cast<unsigned>(reg);

        assert (reg_code < kGeneralPurposeRegistersNumber);

        return general_purpose_regs_[reg_code];
    }

    std::string GetStringReg (GPR reg) const;

    void ExecuteAdd (Instruction instr);

public:
    CpuState () : general_purpose_regs_{}, program_counter_(0) {};

    Word Fetch (BinaryCode& code) const
    {
        return code.GetInstructionEncoding (program_counter_);
    }

    Instruction Decode (Word encoding) const;

    void Execute (Instruction instr);
}; // class CPUState

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------