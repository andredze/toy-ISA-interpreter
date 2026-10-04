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

void CpuState::ExecuteAdd (Instruction instr)
{
    auto value1 = GetRegValue (instr.reg1_);
    auto value2 = GetRegValue (instr.reg2_);

    auto result = value1 + value2;

    SetRegValue (instr.reg3_, result);

    LOG_TRACE_(
        "\nExecuting {}:\n"
        "{} = {} + {}\n"
        "{} = {} + {}",
        GetStringOpcode (Opcode::kAdd),
        GetStringReg (instr.reg1_),
        GetStringReg (instr.reg2_),
        GetStringReg (instr.reg3_),
        value1,
        value2,
        result
    );
}

//--------------------------------------------------------------------------------

void CpuState::Execute (Instruction instr)
{
    switch (instr.opcode_)
    {
    case Opcode::kSyscall: break;
    case Opcode::kBext:    break;
    case Opcode::kLd:      break;
    case Opcode::kSt:      break;
    case Opcode::kBeq:     break;
    case Opcode::kJ:       break;
    case Opcode::kRori:    break;
    case Opcode::kAddi:    break;
    case Opcode::kStp:     break;
    case Opcode::kXor:     break;
    case Opcode::kMovn:    break;
    case Opcode::kSsat:    break;
    case Opcode::kAdd:     ExecuteAdd (instr); break;
    case Opcode::kCls:     break;
    case Opcode::kLi:      break;
    case Opcode::kUnknown:
    default:
        throw std::runtime_error ("Failed to execute: Unknown instruction opcode");
    }
}

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------