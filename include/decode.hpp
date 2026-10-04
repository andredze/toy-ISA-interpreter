#pragma once

//--------------------------------------------------------------------------------

#include <cstdint>
#include "config.hpp"

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

constexpr std::size_t kSyscallCodeExit = 60;

//==================================================

using GPRValue = std::int32_t;
using PCValue  = std::uint32_t;

constexpr std::size_t kBitsInByte      = 8;
constexpr std::size_t kRegLengthInBits = sizeof(GPRValue) * kBitsInByte;

// General Purpose Registers
enum class GPR : std::uint8_t
{
    kX0, // return value
    kX1, // arg0
    kX2, // arg1
    kX3, // arg2
    kX4, // arg3
    kX5, // arg4
    kX6, // arg5
    kX7, // arg6
    kX8, // syscall number
    kX9,  kX10, kX11, kX12, 
    kX13, kX14, kX15, kX16, 
    kX17, kX18, kX19, kX20,
    kX21, kX22, kX23, kX24,
    kX25, kX26, kX27, kX28,
    kX29, kX30,
    kX31, // LR (link register, for return address)
    kUnknown
};

//--------------------------------------------------------------------------------

constexpr std::size_t kOpcodeLength      = 6;
constexpr std::size_t kOpcodeLowBitsMask = 0b111'111;

//==================================================

enum class Opcode : std::uint8_t
{
    kUnknown = 0,
    kSyscall,

    kBext,
    kLd,
    kSt,
    kBeq,
    kJ,
    kRori,
    kAddi,
    kStp,
    kXor,
    kMovn,
    kSsat,
    kAdd,
    kCls,
    kLi
};

//==================================================

enum class BinaryOpcodeHighBits : std::uint8_t
{
    kUnknown = 0b000000,
    
    kJ    = 0b001111,
    kSsat = 0b010010,
    kAddi = 0b010111,
    kLd   = 0b011100,
    kLi   = 0b011101,
    kBeq  = 0b100111,
    kRori = 0b101100,
    kSt   = 0b110000,
    kStp  = 0b111011
};

//==================================================

enum class BinaryOpcodeLowBits : std::uint8_t
{
    kUnknown = 0b000000,
    
    kSyscall = 0b001101,
    kMovn    = 0b011001,
    kAdd     = 0b011011,
    kCls     = 0b101000,
    kBext    = 0b111001,
    kXor     = 0b111110
};

//==================================================

struct FieldLocation
{
    uint8_t start_pos_{};
    uint8_t end_pos_{};
};

//==================================================

uint8_t GetFieldWidth (FieldLocation location);

//==================================================

constexpr FieldLocation kSyscallCodeFieldLocation      = {6, 25};

constexpr FieldLocation kFirstRegisterFieldLocation    = {21, 26};
constexpr FieldLocation kSecondRegisterFieldLocation   = {16, 20};
constexpr FieldLocation kThirdRegisterFieldLocation    = {11, 15};

constexpr FieldLocation kBextZeroFieldLocation         = {6, 10};
constexpr FieldLocation kLdZeroFieldLocation           = {14, 15};
constexpr FieldLocation kStZeroFieldLocation           = {14, 15};
constexpr FieldLocation kRoriZeroFieldLocation         = {0, 10};
constexpr FieldLocation kSsatZeroFieldLocation         = {0, 10};
constexpr FieldLocation kXorZeroFieldLocation          = {6, 10};
constexpr FieldLocation kMovnZeroFieldLocation         = {6, 10};
constexpr FieldLocation kAddZeroFieldLocation          = {6, 10};
constexpr FieldLocation kClsZeroFieldLocation          = {6, 15};
constexpr FieldLocation kLiZeroFieldLocation           = {21, 25};
 
constexpr FieldLocation kStImmediateFieldLocation      = {0, 13};
constexpr FieldLocation kLdImmediateFieldLocation      = {0, 13};
constexpr FieldLocation kAddiImmediateFieldLocation    = {0, 15};
constexpr FieldLocation kLiImmediateFieldLocation      = {0, 15};
constexpr FieldLocation kShortImmediateFieldLocation   = {11, 15};

constexpr FieldLocation kBeqOffsetFieldLocation        = {0, 15};
constexpr FieldLocation kStpOffsetFieldLocation        = {0, 10};

constexpr FieldLocation kInstructionIndexFieldLocation = {0, 25};

//--------------------------------------------------------------------------------

struct Instruction
{
    Opcode opcode_{Opcode::kUnknown};
    GPR reg1_{GPR::kUnknown}, reg2_{GPR::kUnknown}, reg3_{GPR::kUnknown};
    Word imm_{};
};

//==================================================

std::string GetStringOpcode (Opcode opcode);

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------