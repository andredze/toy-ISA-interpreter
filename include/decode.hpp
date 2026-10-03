#pragma once

//--------------------------------------------------------------------------------

#include <cstdint>

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

using Word = std::uint32_t;

constexpr std::size_t kWordSizeInBits = sizeof(Word) * 8;
constexpr std::size_t kOpcodeLength   = 6;

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

//--------------------------------------------------------------------------------

struct Instruction
{
    Opcode opcode_{};
    Word src1_{};
    Word src2_{};
    Word dest_{};
    Word syscall_code_{};
    Word base_{};
    Word offset_{};
    Word target1_{};
    Word target2_{};
    Word immediate_{};
};

//==================================================

Instruction Decode (Word encoding);

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------