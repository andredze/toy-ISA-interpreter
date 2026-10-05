#pragma once

//--------------------------------------------------------------------------------

#include "cpu.hpp"
#include <gtest/gtest.h>

//--------------------------------------------------------------------------------

using namespace toy_isa_interpreter;

//--------------------------------------------------------------------------------

TEST(Decoder, Syscall)
{
    CpuState cpu{};

    Word encoding1 = 0b000000'00000'00000'00000'00000'001101;

    Instruction instr1 = cpu.Decode (encoding1);

    EXPECT_EQ (instr1.opcode_, Opcode::kSyscall);
    EXPECT_EQ (instr1.reg1_,   GPR::kUnknown);
    EXPECT_EQ (instr1.reg2_,   GPR::kUnknown);
    EXPECT_EQ (instr1.reg3_,   GPR::kUnknown);
    EXPECT_EQ (instr1.imm_,    0);

    Word encoding2 = 0b000000'10000'00000'00100'11101'001101;
    Instruction instr2 = cpu.Decode (encoding2);

    EXPECT_EQ (instr2.reg1_, GPR::kUnknown);
    EXPECT_EQ (instr2.reg2_, GPR::kUnknown);
    EXPECT_EQ (instr2.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr2.imm_,  0b10000'00000'00100'11101);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Bext)
{
    CpuState cpu{};

    Word encoding = 0b000000'00001'00010'00011'00000'111001;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kBext);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kX3);
    EXPECT_EQ (instr.imm_,  0);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Ld)
{
    CpuState cpu{};

    Word encoding = 0b011100'00001'00010'00'10000000'111001;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kLd);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr.imm_,  0b10000000'111001);
}

//--------------------------------------------------------------------------------

TEST(Decoder, St)
{
    CpuState cpu{};

    Word encoding = 0b110000'00001'00010'00'10000000'111001;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kSt);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr.imm_,  0b10000000'111001);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Beq)
{
    CpuState cpu{};

    Word encoding = 0b100111'00001'00010'1010000000111001;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kBeq);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr.imm_,  0b1010000000111001);
}

//--------------------------------------------------------------------------------

TEST(Decoder, J)
{
    CpuState cpu{};

    Word encoding = 0b001111'10001000101010000000111001;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kJ);
    EXPECT_EQ (instr.reg1_, GPR::kUnknown);
    EXPECT_EQ (instr.reg2_, GPR::kUnknown);
    EXPECT_EQ (instr.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr.imm_,  0b10001000101010000000111001);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Rori)
{
    CpuState cpu{};

    Word encoding = 0b101100'00001'00010'10100'00000000000;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kRori);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr.imm_,  0b10100);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Addi)
{
    CpuState cpu{};

    Word encoding = 0b010111'00001'00010'1010000000001001;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kAddi);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr.imm_,  0b1010000000001001);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Stp)
{
    CpuState cpu{};

    Word encoding = 0b111011'00001'00010'11111'10000000001;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kStp);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kX31);
    EXPECT_EQ (instr.imm_,  0b10000000001);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Xor)
{
    CpuState cpu{};

    Word encoding = 0b000000'00001'00010'11111'00000'111110;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kXor);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kX31);
    EXPECT_EQ (instr.imm_,  0);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Movn)
{
    CpuState cpu{};

    Word encoding = 0b000000'00001'00010'11111'00000'011001;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kMovn);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kX31);
    EXPECT_EQ (instr.imm_,  0);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Ssat)
{
    CpuState cpu{};

    Word encoding = 0b010010'00001'00010'11111'00000000000;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kSsat);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr.imm_,  0b11111);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Add)
{
    CpuState cpu{};

    Word encoding = 0b000000'00001'00010'11111'00000'011011;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kAdd);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kX31);
    EXPECT_EQ (instr.imm_,  0);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Cls)
{
    CpuState cpu{};

    Word encoding = 0b000000'00001'00010'00000'00000'101000;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kCls);
    EXPECT_EQ (instr.reg1_, GPR::kX1);
    EXPECT_EQ (instr.reg2_, GPR::kX2);
    EXPECT_EQ (instr.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr.imm_,  0);
}

//--------------------------------------------------------------------------------

TEST(Decoder, Li)
{
    CpuState cpu{};

    Word encoding = 0b011101'00000'00011'1000001000101000;

    Instruction instr = cpu.Decode (encoding);

    EXPECT_EQ (instr.opcode_, Opcode::kLi);
    EXPECT_EQ (instr.reg1_, GPR::kX3);
    EXPECT_EQ (instr.reg2_, GPR::kUnknown);
    EXPECT_EQ (instr.reg3_, GPR::kUnknown);
    EXPECT_EQ (instr.imm_,  0b1000001000101000);
}

//--------------------------------------------------------------------------------

