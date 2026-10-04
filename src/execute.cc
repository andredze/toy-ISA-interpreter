#include <unistd.h>
#include "cpu.hpp"
#include "decode.hpp"
#include "logger.hpp"

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

std::string CpuState::GetStringReg (GPR reg) const
{
    switch (reg)
    {
    case GPR::kX0:  return "X0 (ret val)";
    case GPR::kX1:  return "X1 (arg0)"; 
    case GPR::kX2:  return "X2 (arg1)"; 
    case GPR::kX3:  return "X3 (arg2)"; 
    case GPR::kX4:  return "X4 (arg3)"; 
    case GPR::kX5:  return "X5 (arg4)"; 
    case GPR::kX6:  return "X6 (arg5)"; 
    case GPR::kX7:  return "X7 (arg6)"; 
    case GPR::kX8:  return "X8 (syscall number)";
    case GPR::kX9:  return "X9";  
    case GPR::kX10: return "X10"; 
    case GPR::kX11: return "X11";
    case GPR::kX12: return "X12"; 
    case GPR::kX13: return "X13"; 
    case GPR::kX14: return "X14";
    case GPR::kX15: return "X15";
    case GPR::kX16: return "X16"; 
    case GPR::kX17: return "X17"; 
    case GPR::kX18: return "X18";
    case GPR::kX19: return "X19";
    case GPR::kX20: return "X20";
    case GPR::kX21: return "X21"; 
    case GPR::kX22: return "X22";
    case GPR::kX23: return "X23";
    case GPR::kX24: return "X24";
    case GPR::kX25: return "X25"; 
    case GPR::kX26: return "X26";
    case GPR::kX27: return "X27";
    case GPR::kX28: return "X28";
    case GPR::kX29: return "X29"; 
    case GPR::kX30: return "X30";
    case GPR::kX31: return "X31 (LR)";
    case GPR::kUnknown:
    default:
        return "UNKNOWN REGISTER";
    }

    return "UNKNOWN REGISTER";
}

//--------------------------------------------------------------------------------

void CpuState::AdvanceProgramCounter ()
{
    LOG_TRACE_("\nAdvancing PC: PC = {} + 4 = {}", 
                program_counter_, program_counter_ + 4u);

    program_counter_ += 4u;
}

//--------------------------------------------------------------------------------

static GPRValue GetBit (GPRValue value, int bit_index)
{
    return (value >> bit_index) & 1;    
}

//==================================================

static GPRValue SignExtend (GPRValue source, uint8_t width)
{
    GPRValue result = 0;

    if (GetBit (source, width - 1) == 0) {
        result = source;
    }
    else {
        // add leading ones
        result = source | ((~0) << width);
    }

    return result;
}

//==================================================

void CpuState::ExecuteSyscall (Instruction instr)
{
    // SigException(SystemCall)
    // X8 ― system call number, X0 - X7 ― args, X0 ― result, see man syscall

    auto syscall_number = GetRegValue (GPR::kX8);

    auto arg0 = GetRegValue (GPR::kX0);
    auto arg1 = GetRegValue (GPR::kX1);
    auto arg2 = GetRegValue (GPR::kX2);
    auto arg3 = GetRegValue (GPR::kX3);
    auto arg4 = GetRegValue (GPR::kX4);
    auto arg5 = GetRegValue (GPR::kX5);
    auto arg6 = GetRegValue (GPR::kX6);
    auto arg7 = GetRegValue (GPR::kX7);

    LOG_TRACE_(
        "\nExecuting {}\n"
        "Raising syscall {} with arguments\n"
        "{}, {}, {}, {},\n"
        "{}, {}, {}, {}\n",
        GetStringOpcode (Opcode::kSyscall),
        syscall_number,
        arg0, arg1, arg2, arg3,
        arg4, arg5, arg6, arg7
    );

    auto result = static_cast<GPRValue>(
    syscall (
        syscall_number,
        arg0, arg1, arg2, arg3,
        arg4, arg5, arg6, arg7
    ));

    SetRegValue (GPR::kX0, result);

    LOG_TRACE_(
        "\nResult of a syscall: {}\n",
        result
    );
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteBext (Instruction instr)
{
    // X[reg1] ← bit_extract(X[reg2], X[reg3])

    GPR reg_data   = instr.reg2_;
    GPR reg_mask   = instr.reg3_;
    GPR reg_result = instr.reg1_;

    auto data = GetRegValue (reg_data);
    auto mask = GetRegValue (reg_mask);

    GPRValue packed_bits = 0u;
    
    for (int bit_index = kRegLengthInBits - 1; bit_index >= 0; bit_index--) {
        if (GetBit (mask, bit_index) == 1u) {
            packed_bits <<= 1;
            packed_bits |= (GetBit (data, bit_index));
        }
    }

    SetRegValue (reg_result, packed_bits);

    LOG_TRACE_(
        "\nExecuting {}:\n"
        "{} = bit_extract(data {}, mask {})\n"
        "data {}   = {:032b}\n"
        "mask {}   = {:032b}\n"
        "result {} = {:032b}\n",
        GetStringOpcode (Opcode::kBext),
        GetStringReg (reg_result),
        GetStringReg (reg_data),
        GetStringReg (reg_mask),
        GetStringReg (reg_data), data,
        GetStringReg (reg_mask), mask,
        GetStringReg (reg_result), packed_bits
    );

    AdvanceProgramCounter ();
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteLd (Instruction instr)
{
    // base = reg1
    // rt   = reg2
    // addr ← X[base] + sign_extend(imm)
    // raise MisalignedAccess unless isAligned(addr)
    // X[rt] ← memory[addr]

    auto reg_base = instr.reg1_;
    auto reg_dest = instr.reg2_;
    auto imm = instr.imm_;

    auto addr = GetRegValue (reg_base) + 
                SignExtend  (imm, GetFieldWidth (kLdImmediateFieldLocation));

    // TODO: uncomment when implement memory
    // if (!IsAligned (addr)) {
    //     throw std::runtime_error ("MisalignedAccess");
    // }

    // GPRValue result = MemoryLoadWord (memory, addr);
    
    // SetRegValue (reg_dest, result);
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteSt (Instruction instr)
{
    // base = reg1
    // rt   = reg2
    // addr ← X[base] + sign_extend(imm)
    // raise MisalignedAccess unless isAligned(addr)
    // memory[addr] ← X[rt]

    auto reg_base = instr.reg1_;
    auto reg_src  = instr.reg2_;
    auto imm = instr.imm_;

    auto addr = GetRegValue (reg_base) + 
                SignExtend  (imm, GetFieldWidth (kStImmediateFieldLocation));

    // TODO: uncomment when implement memory
    // if (!IsAligned (addr)) {
    //     throw std::runtime_error ("MisalignedAccess");
    // }

    auto value = GetRegValue (reg_src);

    // MemoryStoreWord (memory, addr, value);
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteBeq (Instruction instr)
{
    // target ← sign_extend(offset) << 2
    // cond ← X[reg1] == X[reg2]
    // PC ← if (cond) PC + target else PC + 4

    auto value1 = GetRegValue (instr.reg1_);
    auto value2 = GetRegValue (instr.reg2_);
    auto offset = static_cast<GPRValue>(instr.imm_);

    auto target = SignExtend (offset, GetFieldWidth (kBeqOffsetFieldLocation)) << 2;
    bool cond   = (value1 == value2);

    auto pc_value = GetProgramCounter ();

    if (target < 0 && (-target) > pc_value) {
        throw std::runtime_error ("Jump to negative address");
    }

    auto result = pc_value + target;

    LOG_TRACE_(
        "\nExecuting {}\n"
        "target = {}\n"
        "check condition:\n"
        "{} with value {}\n"
        "{} with value {}\n"
        "cond equals = {}\n"
        "if true: new PC = {}\n",
        GetStringOpcode (Opcode::kBeq),
        target,
        GetStringReg (instr.reg1_), value1,
        GetStringReg (instr.reg2_), value2,
        cond,
        result
    );

    if (cond) {
        SetProgramCounter (result);
    }
    else {
        AdvanceProgramCounter ();
    }
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteJ (Instruction instr)
{
    // PC ← (PC & 0xF0000000) | (instr_index << 2)

    auto instr_index = instr.imm_;

    auto pc_value = GetProgramCounter ();

    auto result = (pc_value & 0xF000'0000) | (instr_index << 2);

    SetProgramCounter (result);

    LOG_TRACE_(
        "\nExecuting {}\n"
        "PC = {} + 4 * {} = {}\n",
        GetStringOpcode (Opcode::kJ),
        pc_value, instr_index, result
    );
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteRori (Instruction instr)
{   
    // X[reg1] ← rotate_right(X[reg2], imm5)

    GPR reg_result = instr.reg1_;
    GPR reg_source = instr.reg2_;

    auto value = GetRegValue (reg_source);

    auto rotate_count = instr.imm_;

    auto result = (value >> rotate_count) | 
                  (value << (kRegLengthInBits - rotate_count));
    
    SetRegValue (reg_result, result);

    LOG_TRACE_(
        "\nExecuting {}:\n"
        "Rotating by {} bits:\n"
        "source: {} = {:032b}\n"
        "result: {} = {:032b}\n",
        GetStringOpcode (Opcode::kRori),
        rotate_count,
        GetStringReg (reg_source), value,
        GetStringReg (reg_result), result
    );

    AdvanceProgramCounter ();
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteAddi (Instruction instr)
{   
    // X[reg2] ← X[reg1] + sign_extend(imm)

    GPR reg_result = instr.reg2_;
    GPR reg_source = instr.reg1_;

    auto value1 = GetRegValue (reg_source);

    auto imm = static_cast<GPRValue>(instr.imm_);

    auto sign_extended = SignExtend (imm, GetFieldWidth (kAddiImmediateFieldLocation));    
   
    auto result = value1 + sign_extended;

    SetRegValue (reg_result, result);

    LOG_TRACE_(
        "\nExecuting {}:\n"
        "source1: {} = {:032b}\n"
        "imm: {:016b}\n"
        "sign_extended_imm: {:032b}\n"
        "result: {} = {:032b}\n",
        GetStringOpcode (Opcode::kAddi),
        GetStringReg (reg_source), value1,
        imm,
        sign_extended,
        GetStringReg (reg_result), result
    );

    AdvanceProgramCounter ();
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteStp (Instruction instr)
{
    // base = reg1
    // rt1  = reg2
    // rt2  = reg3
    // addr ← X[base] + sign_extend(offset)
    // raise MisalignedAccess unless isAligned(addr)
    // memory[addr] ← X[rt1]
    // memory[addr + 4] ← X[rt2]

    auto reg_base = instr.reg1_;
    auto reg_src1 = instr.reg2_;
    auto reg_src2 = instr.reg3_;
    auto offset   = instr.imm_;

    auto addr = GetRegValue (reg_base) + 
                SignExtend  (offset, GetFieldWidth (kStpOffsetFieldLocation));

    // TODO: uncomment when implement memory
    // if (!IsAligned (addr)) {
    //     throw std::runtime_error ("MisalignedAccess");
    // }

    auto value1 = GetRegValue (reg_src1);
    auto value2 = GetRegValue (reg_src2);

    // MemoryStoreWord (memory, addr,     value1);
    // MemoryStoreWord (memory, addr + 4, value2);
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteXor (Instruction instr)
{
    // X[reg3] ← X[reg1] ^ X[reg2]

    auto value1 = GetRegValue (instr.reg1_);
    auto value2 = GetRegValue (instr.reg2_);;

    GPR reg_result = instr.reg3_;

    auto result = value1 ^ value2;

    SetRegValue (reg_result, result);

    LOG_TRACE_(
        "\nExecuting {}:\n"
        "{} = {} ^ {}\n"
        "{} = {} ^ {}",
        GetStringOpcode (Opcode::kXor),
        GetStringReg (reg_result),
        GetStringReg (instr.reg1_),
        GetStringReg (instr.reg2_),
        result,
        value1,
        value2
    );

    AdvanceProgramCounter ();
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteMovn (Instruction instr)
{
    // if (X[reg2] != 0) X[reg3] ← X[reg1]

    auto value      = GetRegValue (instr.reg1_);
    auto cond_value = GetRegValue (instr.reg2_);

    if (cond_value != 0) {
        SetRegValue (instr.reg3_, value);
    }

    LOG_TRACE_(
        "\nExecuting {}:\n"
        "if ({} != 0) {} ← {}\n"
        "({} ({}) != 0) is {}\n"
        "so: {} = {}\n",
        GetStringOpcode (Opcode::kMovn),
        GetStringReg (instr.reg2_),
        GetStringReg (instr.reg3_),
        GetStringReg (instr.reg1_),
        GetStringReg (instr.reg2_),
        cond_value,
        (cond_value != 0),
        GetStringReg (instr.reg3_),
        GetRegValue (instr.reg3_)
    );

    AdvanceProgramCounter ();
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteSsat (Instruction instr)
{
    // X[reg1] ← saturate_signed(X[reg2], imm5)

    GPR reg_value  = instr.reg2_;
    GPR reg_result = instr.reg1_;
    
    auto bits_count = instr.imm_;

    auto max_value = (1 << bits_count) - 1;
    auto min_value = - (1 << bits_count);

    auto value = GetRegValue (reg_value);

    if (value > max_value) {
        value = max_value;
    }
    else if (value < min_value) {
        value = min_value;
    }

    SetRegValue (reg_result, value);

    LOG_TRACE_(
        "\nExecuting {}:\n"
        "bits_count = {}\n"
        "max_value = {}\n"
        "min_value = {}\n"
        "value before saturation = {} (from {})\n"
        "result value = {} = {}\n",
        GetStringOpcode (Opcode::kSsat),
        bits_count,
        max_value,
        min_value,
        GetRegValue (reg_value), GetStringReg (reg_value),
        value, GetStringReg (reg_result)
    );

    AdvanceProgramCounter ();
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteAdd (Instruction instr)
{
    // X[reg3] ← X[reg1] + X[reg2]

    auto value1 = GetRegValue (instr.reg1_);
    auto value2 = GetRegValue (instr.reg2_);

    auto result = value1 + value2;

    SetRegValue (instr.reg3_, result);

    LOG_TRACE_(
        "\nExecuting {}:\n"
        "{} = {} + {}\n"
        "{} = {} + {}",
        GetStringOpcode (Opcode::kAdd),
        GetStringReg (instr.reg3_),
        GetStringReg (instr.reg1_),
        GetStringReg (instr.reg2_),
        result,
        value1,
        value2
    );

    AdvanceProgramCounter ();
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteCls (Instruction instr)
{
    // X[reg1] ← count_leading_signs(X[reg2])

    GPR reg_result = instr.reg1_;
    GPR reg_source = instr.reg2_;

    auto value = GetRegValue (reg_source);

    GPRValue leading_ones_count = 0;

    for (int bit_index = kRegLengthInBits - 1; bit_index >= 0; bit_index--) {
        if (GetBit (value, bit_index) == 0) {
            break;
        }

        leading_ones_count++;
    }

    SetRegValue (reg_result, leading_ones_count);

    LOG_TRACE_(
        "\nExecuting {}\n"
        "source: {} = {:032b}\n"
        "leading_ones = {}\n"
        "write back: {} = {}\n",
        GetStringOpcode (Opcode::kCls),
        GetStringReg (reg_source), value,
        leading_ones_count,
        GetStringReg (reg_result), GetRegValue (reg_result)
    );

    AdvanceProgramCounter ();
}

//--------------------------------------------------------------------------------

void CpuState::ExecuteLi (Instruction instr)
{
    // X[reg1] ← sign_extend(imm)
    GPR reg = instr.reg1_;

    GPRValue imm = static_cast<GPRValue>(instr.imm_);

    auto imm_width = GetFieldWidth (kLiImmediateFieldLocation);

    GPRValue result = SignExtend (imm, imm_width);

    SetRegValue (reg, result);

    LOG_TRACE_(
        "\nExecuting {}\n"
        "imm = {:016b}\n"
        "result = {:032b}\n"
        "write back: {} = {}\n",
        GetStringOpcode (Opcode::kLi),
        imm,
        result,
        GetStringReg (reg), GetRegValue (reg)
    );

    AdvanceProgramCounter ();
}

//--------------------------------------------------------------------------------

void CpuState::Execute (Instruction instr)
{
    switch (instr.opcode_)
    {
    case Opcode::kSyscall: ExecuteSyscall (instr); break;
    case Opcode::kBext:    ExecuteBext    (instr); break;
    case Opcode::kLd:      ExecuteLd      (instr); break;
    case Opcode::kSt:      ExecuteSt      (instr); break;
    case Opcode::kBeq:     ExecuteBeq     (instr); break;
    case Opcode::kJ:       ExecuteJ       (instr); break;
    case Opcode::kRori:    ExecuteRori    (instr); break;
    case Opcode::kAddi:    ExecuteAddi    (instr); break;
    case Opcode::kStp:     ExecuteStp     (instr); break;
    case Opcode::kXor:     ExecuteXor     (instr); break;
    case Opcode::kMovn:    ExecuteMovn    (instr); break;
    case Opcode::kSsat:    ExecuteSsat    (instr); break;
    case Opcode::kAdd:     ExecuteAdd     (instr); break;
    case Opcode::kCls:     ExecuteCls     (instr); break;
    case Opcode::kLi:      ExecuteLi      (instr); break;
    case Opcode::kUnknown:
    default:
        throw std::runtime_error ("Failed to execute: Unknown instruction opcode");
    }
}

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------