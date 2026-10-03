#include "decode.hpp"
#include "logger.hpp"
#include <cassert>
#include <stdexcept>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

/*
    In our ISA encoding opcode can be stored two ways:
        1st: opcode is in [31:26] bits
        2nd: [31:26] bits are zeroes, and opcode is in [5:0] bits
*/

static enum Opcode GetOpcodeFromHighBits (Word high_opcode_bits)
{
    assert (high_opcode_bits != 0);

    enum Opcode opcode = Opcode::kUnknown;
    enum BinaryOpcodeHighBits bin_opcode = static_cast<BinaryOpcodeHighBits>(high_opcode_bits);

    switch (bin_opcode)
    {
    case BinaryOpcodeHighBits::kJ:       return Opcode::kJ;
    case BinaryOpcodeHighBits::kSsat:    return Opcode::kSsat;
    case BinaryOpcodeHighBits::kAddi:    return Opcode::kAddi;
    case BinaryOpcodeHighBits::kLd:      return Opcode::kLd;
    case BinaryOpcodeHighBits::kLi:      return Opcode::kLi;
    case BinaryOpcodeHighBits::kBeq:     return Opcode::kBeq;
    case BinaryOpcodeHighBits::kRori:    return Opcode::kRori;
    case BinaryOpcodeHighBits::kSt:      return Opcode::kSt;
    case BinaryOpcodeHighBits::kStp:     return Opcode::kStp;
    case BinaryOpcodeHighBits::kUnknown:
    default:
        throw std::runtime_error("Unknown instruction opcode");
        break;
    }

    return opcode;
}

//————————————————————————————————————————————————————————————————————————————————

static enum Opcode GetOpcodeFromLowBits (Word low_opcode_bits)
{
    enum Opcode opcode = Opcode::kUnknown;
    enum BinaryOpcodeLowBits bin_opcode = static_cast<BinaryOpcodeLowBits>(low_opcode_bits);

    switch (bin_opcode)
    {
    case BinaryOpcodeLowBits::kSyscall: return Opcode::kSyscall;
    case BinaryOpcodeLowBits::kMovn:    return Opcode::kMovn;
    case BinaryOpcodeLowBits::kAdd:     return Opcode::kAdd;
    case BinaryOpcodeLowBits::kCls:     return Opcode::kCls;
    case BinaryOpcodeLowBits::kBext:    return Opcode::kBext;
    case BinaryOpcodeLowBits::kXor:     return Opcode::kXor;
    case BinaryOpcodeLowBits::kUnknown:
    default:
        throw std::runtime_error("Unknown instruction opcode");
        break;
    }

    return opcode;
}

//————————————————————————————————————————————————————————————————————————————————

static enum Opcode GetOpcode (Word encoding)
{
    Word high_opcode_bits = (encoding >> (kWordSizeInBits - kOpcodeLength));

    enum Opcode opcode = Opcode::kUnknown;

    if (high_opcode_bits == 0) {
        Word low_opcode_bits = (encoding & kOpcodeLowBitsMask);

        opcode = GetOpcodeFromLowBits (low_opcode_bits);

        LOG_TRACE_("Decode opcode {} from low bits {:06b} encoding {:032b}",
                    static_cast<unsigned>(opcode), low_opcode_bits, encoding);    
    }
    else {
        opcode = GetOpcodeFromHighBits (high_opcode_bits);

        LOG_TRACE_("Decode opcode {} from high bits {:06b} encoding {:032b}",
                    static_cast<unsigned>(opcode), high_opcode_bits, encoding);    
    }

    return opcode;
}

//--------------------------------------------------------------------------------

static Word GetSyscallCode (Word encoding)
{
    assert (GetOpcode (encoding) == Opcode::kSyscall);

    Word syscall_code = ((encoding << kOpcodeLength) >> 2 * kOpcodeLength);

    LOG_TRACE_("Decode syscall code {:020b} (decimal {:d}) from encoding {:032b}",
                syscall_code, syscall_code, encoding);

    return syscall_code;
}

//————————————————————————————————————————————————————————————————————————————————

Instruction Decode (Word encoding)
{
    Opcode opcode = GetOpcode (encoding);

    Instruction instr{.opcode_ = opcode};

    switch (instr.opcode_)
    {
    case Opcode::kSyscall:
        instr.syscall_code_ = GetSyscallCode (encoding);
        break;

    case Opcode::kBext:
        break;

    case Opcode::kLd:
        break;
    case Opcode::kSt:
        break;
    case Opcode::kBeq:
        break;
    case Opcode::kJ:
        break;
    case Opcode::kRori:
        break;
    case Opcode::kAddi:
        break;
    case Opcode::kStp:
        break;
    case Opcode::kXor:
        break;
    case Opcode::kMovn:
        break;
    case Opcode::kSsat:
        break;
    case Opcode::kAdd:
        break;
    case Opcode::kCls:
        break;
    case Opcode::kLi:
        break;

    case Opcode::kUnknown:
    default:
        break;
    }

    return instr;
}

//————————————————————————————————————————————————————————————————————————————————

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————