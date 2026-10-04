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
    Word GetInstructionEncoding (PCValue program_counter)
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
    GPRValue general_purpose_regs_[kGeneralPurposeRegistersNumber];
    PCValue  program_counter_;

    //==================================================

    void SetRegValue (GPR reg, GPRValue value)
    {
        unsigned reg_code = static_cast<unsigned>(reg);

        assert (reg_code < kGeneralPurposeRegistersNumber);

        general_purpose_regs_[reg_code] = value;
    }

    GPRValue GetRegValue (GPR reg) const
    {
        unsigned reg_code = static_cast<unsigned>(reg);

        assert (reg_code < kGeneralPurposeRegistersNumber);

        return general_purpose_regs_[reg_code];
    }

    std::string GetStringReg (GPR reg) const;

    PCValue GetProgramCounter () const
    {
        return program_counter_;
    }

    void SetProgramCounter (PCValue value)
    {
        program_counter_ = value;
    }

    void AdvanceProgramCounter ();

    //==================================================

    void ExecuteBext  (Instruction instr);
    void ExecuteBeq   (Instruction instr);
    void ExecuteJ     (Instruction instr);
    void ExecuteRori  (Instruction instr);
    void ExecuteAddi  (Instruction instr);
    void ExecuteXor   (Instruction instr);
    void ExecuteMovn  (Instruction instr);
    void ExecuteSsat  (Instruction instr);
    void ExecuteAdd   (Instruction instr);
    void ExecuteCls   (Instruction instr);
    void ExecuteLi    (Instruction instr);

    //--------------------------------------------------------------------------------

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