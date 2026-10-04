#include "decode.hpp"
#include "cpu.hpp"
#include "logger.hpp"
#include <cassert>
#include <stdexcept>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

std::string GetStringOpcode (Opcode opcode)
{
    switch (opcode)
    {
    case Opcode::kSyscall: return "SYSCALL";
    case Opcode::kBext:    return "BEXT";
    case Opcode::kLd:      return "LD";
    case Opcode::kSt:      return "ST";
    case Opcode::kBeq:     return "BEQ";
    case Opcode::kJ:       return "J";
    case Opcode::kRori:    return "RORI";
    case Opcode::kAddi:    return "ADDI";
    case Opcode::kStp:     return "STP";
    case Opcode::kXor:     return "XOR";
    case Opcode::kMovn:    return "MOVN";
    case Opcode::kSsat:    return "SSAT";
    case Opcode::kAdd:     return "ADD";
    case Opcode::kCls:     return "CLS";
    case Opcode::kLi:      return "LI";
    
    case Opcode::kUnknown:
    default:
        return "UNKNOWN";
    }

    return "UNKNOWN";    
}

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
                    GetStringOpcode (opcode), low_opcode_bits, encoding);    
    }
    else {
        opcode = GetOpcodeFromHighBits (high_opcode_bits);

        LOG_TRACE_("Decode opcode {} from high bits {:06b} encoding {:032b}",
                    GetStringOpcode (opcode), high_opcode_bits, encoding);    
    }

    return opcode;
}

//--------------------------------------------------------------------------------

static Word GetField (Word encoding, FieldLocation location)
{
    assert (location.end_pos_ <= sizeof(Word) * 8);
    assert (location.end_pos_ >= location.start_pos_);

    uint8_t width = location.end_pos_ - location.start_pos_ + 1u;

    Word remove_upper_bits_mask = (1u << width) - 1;

    return (encoding >> location.start_pos_) & (remove_upper_bits_mask);
}

//--------------------------------------------------------------------------------

static Word GetSyscallCode (Word encoding)
{
    assert (GetOpcode (encoding) == Opcode::kSyscall);

    Word syscall_code = GetField (encoding, kSyscallCodeFieldLocation);

    LOG_TRACE_("Decode syscall code {:020b} (decimal {:d}) from encoding {:032b}",
                syscall_code, syscall_code, encoding);

    return syscall_code;
}

//--------------------------------------------------------------------------------

/*
    according to specification some fields should contain only zeroes
*/
static void EnsureFieldIsZero (Word encoding, FieldLocation location)
{
    if (GetField (encoding, location) != 0) {
        throw std::runtime_error ("Unknown instruction");
    }
}

//--------------------------------------------------------------------------------

static GPR GetFirstFieldReg (Word encoding)
{
    return static_cast<GPR>(GetField (encoding, kFirstRegisterFieldLocation));
}

//--------------------------------------------------------------------------------

static GPR GetSecondFieldReg (Word encoding)
{
    return static_cast<GPR>(GetField (encoding, kSecondRegisterFieldLocation));
}

//--------------------------------------------------------------------------------

static GPR GetThirdFieldReg (Word encoding)
{
    return static_cast<GPR>(GetField (encoding, kThirdRegisterFieldLocation));
}

//--------------------------------------------------------------------------------

static Word GetShortImmediate (Word encoding)
{
    return GetField (encoding, kShortImmediateFieldLocation);
}

//————————————————————————————————————————————————————————————————————————————————

Instruction CpuState::Decode (Word encoding) const
{
    Opcode opcode = GetOpcode (encoding);

    Instruction instr{.opcode_ = opcode};

    switch (instr.opcode_)
    {
    case Opcode::kSyscall:
        instr.imm_ = GetSyscallCode (encoding);
        break;

    case Opcode::kBext:
        EnsureFieldIsZero (encoding, kBextZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg  (encoding); 
        instr.reg2_ = GetSecondFieldReg (encoding); 
        instr.reg3_ = GetThirdFieldReg  (encoding); 
        break;

    case Opcode::kLd:
        EnsureFieldIsZero (encoding, kLdZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.imm_  = GetField (encoding, kLdImmediateFieldLocation);
        break;

    case Opcode::kSt:
        EnsureFieldIsZero (encoding, kStZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.imm_  = GetField (encoding, kStImmediateFieldLocation);
        break;

    case Opcode::kBeq:
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.imm_  = GetField (encoding, kBeqOffsetFieldLocation);
        break;

    case Opcode::kJ:
        instr.imm_ = GetField (encoding, kInstructionIndexFieldLocation);
        break;

    case Opcode::kRori:
        EnsureFieldIsZero (encoding, kRoriZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.imm_  = GetShortImmediate (encoding);
        break;

    case Opcode::kAddi:
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.imm_  = GetField (encoding, kAddiImmediateFieldLocation);
        break;

    case Opcode::kStp:
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.reg3_ = GetThirdFieldReg  (encoding);
        instr.imm_  = GetField (encoding, kStpOffsetFieldLocation);
        break;

    case Opcode::kXor:
        EnsureFieldIsZero (encoding, kXorZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.reg3_ = GetThirdFieldReg  (encoding);
        break;

    case Opcode::kMovn:
        EnsureFieldIsZero (encoding, kMovnZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.reg3_ = GetThirdFieldReg  (encoding);
        break;

    case Opcode::kSsat:
        EnsureFieldIsZero (encoding, kSsatZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.imm_  = GetShortImmediate (encoding);
        break;

    case Opcode::kAdd:
        EnsureFieldIsZero (encoding, kAddZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        instr.reg3_ = GetThirdFieldReg  (encoding);
        break;

    case Opcode::kCls:
        EnsureFieldIsZero (encoding, kClsZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg  (encoding);
        instr.reg2_ = GetSecondFieldReg (encoding);
        break;

    case Opcode::kLi:
        EnsureFieldIsZero (encoding, kLiZeroFieldLocation);
        instr.reg1_ = GetFirstFieldReg (encoding);
        instr.imm_  = GetField (encoding, kLiImmediateFieldLocation);
        break;

    case Opcode::kUnknown:
    default:
        LOG_ERROR_("Reached point that can not be reached, "
                   "unknown value in opcode enum switch");

        throw std::runtime_error ("Unknown instruction opcode");
    }

    LOG_TRACE_(
        "\nDecoded an instruction:"
        "from encoding: {:032b}\n"
        "opcode = {}\n"
        "reg1_  = {} {:05b}\n"
        "reg2_  = {} {:05b}\n"
        "reg3_  = {} {:05b}\n"
        "imm_   = {:026b}",
        encoding,
        GetStringOpcode (instr.opcode_),
        GetStringReg    (instr.reg1_), static_cast<uint8_t>(instr.reg1_),
        GetStringReg    (instr.reg2_), static_cast<uint8_t>(instr.reg2_),
        GetStringReg    (instr.reg3_), static_cast<uint8_t>(instr.reg3_),
        instr.imm_
    );

    return instr;
}

//————————————————————————————————————————————————————————————————————————————————

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————