#pragma once

//--------------------------------------------------------------------------------

#include <cstdint>
#include "config.hpp"

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

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
    uint8_t start_pos_;
    uint8_t end_pos_;
};

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
    Opcode opcode_;
    Byte reg1_, reg2_, reg3_;
    Word imm_;
};

//==================================================

std::string GetStringOpcode (Opcode opcode);

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------