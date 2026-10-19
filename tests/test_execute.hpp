#pragma once

//--------------------------------------------------------------------------------

#include "interpreter.hpp"
#include <gtest/gtest.h>

//--------------------------------------------------------------------------------

struct RegState
{
    GPR reg;

    GPRValue initial;
    GPRValue expected;
};

//==================================================

struct TestCpuState
{
    Opcode opcode;

    RegState reg_state1;
    RegState reg_state2;
    RegState reg_state3;

    Word imm;

    PCValue pc_initial;
    PCValue pc_expected;
};

//==================================================

class NoMemoryUsageNoExceptionsExecutionTest : public ::testing::TestWithParam<TestCpuState> {};

//--------------------------------------------------------------------------------

TEST_P(NoMemoryUsageNoExceptionsExecutionTest, canExecuteInstructionsWithoutMemoryUsageOrExceptions)
{
    Interpreter inter{};
    CpuState& cpu = inter.cpu_;
    RAM& ram = inter.ram_;

    const TestCpuState& state = GetParam ();

    cpu.SetProgramCounter (state.pc_initial);

    if (state.reg_state1.reg != GPR::kUnknown) {
        cpu.SetRegValue (state.reg_state1.reg, state.reg_state1.initial);
    }
    if (state.reg_state2.reg != GPR::kUnknown) {
        cpu.SetRegValue (state.reg_state2.reg, state.reg_state2.initial);
    }
    if (state.reg_state3.reg != GPR::kUnknown) {
        cpu.SetRegValue (state.reg_state3.reg, state.reg_state3.initial);
    }

    Instruction instr = {
        .opcode_ = state.opcode,
        .reg1_   = state.reg_state1.reg,
        .reg2_   = state.reg_state2.reg,
        .reg3_   = state.reg_state3.reg,
        .imm_    = state.imm
    };

    cpu.Execute (instr, ram);

    if (state.reg_state1.reg != GPR::kUnknown) {
        EXPECT_EQ (cpu.GetRegValue (state.reg_state1.reg), state.reg_state1.expected);
    }
    if (state.reg_state2.reg != GPR::kUnknown) {
        EXPECT_EQ (cpu.GetRegValue (state.reg_state2.reg), state.reg_state2.expected);
    }
    if (state.reg_state3.reg != GPR::kUnknown) {
        EXPECT_EQ (cpu.GetRegValue (state.reg_state3.reg), state.reg_state3.expected);
    }

    EXPECT_EQ (cpu.GetProgramCounter (), state.pc_expected);
}

//--------------------------------------------------------------------------------

INSTANTIATE_TEST_SUITE_P
(
    ExecuteInstructions, 
    NoMemoryUsageNoExceptionsExecutionTest,
    ::testing::Values (
        TestCpuState {
            .opcode      = Opcode::kBext, 
            .reg_state1  = {GPR::kX0, 77777,            0b110001}, 
            .reg_state2  = {GPR::kX1, 0b1111'0000'0111, 0b1111'0000'0111}, 
            .reg_state3  = {GPR::kX2, 0b0110'1100'1100, 0b0110'1100'1100}, 
            .imm         = 0,
            .pc_initial  = 0x04,
            .pc_expected = 0x08
        },
        TestCpuState {
            .opcode      = Opcode::kBeq, 
            .reg_state1  = {GPR::kX0, 15, 15}, 
            .reg_state2  = {GPR::kX1, 15, 15}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 16,
            .pc_initial  = 0x04,
            .pc_expected = 0x44
        },
        TestCpuState {
            .opcode      = Opcode::kBeq, 
            .reg_state1  = {GPR::kX0, 15, 15}, 
            .reg_state2  = {GPR::kX1, 15, 15}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 0b1111'1111'1111'0000, // -16
            .pc_initial  = 0x44,
            .pc_expected = 0x04
        },
        TestCpuState {
            .opcode      = Opcode::kBeq, 
            .reg_state1  = {GPR::kX0, 15, 15}, 
            .reg_state2  = {GPR::kX1, 25, 25}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 0b1111'1111'1111'0000, // -16
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kJ, 
            .reg_state1  = {GPR::kUnknown, 0, 0}, 
            .reg_state2  = {GPR::kUnknown, 0, 0}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 0b1111'1111'1111'00,
            .pc_initial  = 0x44,
            .pc_expected = 0b1111'1111'1111'0000
        },
        TestCpuState {
            .opcode      = Opcode::kRori, 
            .reg_state1  = {GPR::kX1, 40, (1 << 31)}, 
            .reg_state2  = {GPR::kX2, 1, 1}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 1,
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kRori, 
            .reg_state1  = {GPR::kX1, 40, (0b101 << 29) | (0b11)}, 
            .reg_state2  = {GPR::kX2, 0b0001'1101, 0b0001'1101}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 3,
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kAddi, 
            .reg_state1  = {GPR::kX1, 40, 40}, 
            .reg_state2  = {GPR::kX2, 77777, 55}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 15,
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kAddi, 
            .reg_state1  = {GPR::kX1, 40, 40}, 
            .reg_state2  = {GPR::kX2, 77777, 25}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 0b1111'1111'1111'0001,
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kXor, 
            .reg_state1  = {GPR::kX1, 0b0000'1111'1010, 0b0000'1111'1010}, 
            .reg_state2  = {GPR::kX2, 0b1100'1010'1111, 0b1100'1010'1111}, 
            .reg_state3  = {GPR::kX3, 77777,            0b1100'0101'0101}, 
            .imm         = 0,
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kMovn, 
            .reg_state1  = {GPR::kX1, 66666, 66666}, 
            .reg_state2  = {GPR::kX2, 1, 1}, 
            .reg_state3  = {GPR::kX3, 77777, 66666}, 
            .imm         = 0,
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kMovn, 
            .reg_state1  = {GPR::kX1, 66666, 66666}, 
            .reg_state2  = {GPR::kX2, 0, 0}, 
            .reg_state3  = {GPR::kX3, 77777, 77777}, 
            .imm         = 0,
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kSsat, 
            .reg_state1  = {GPR::kX1, 77777, -16}, 
            .reg_state2  = {GPR::kX2, -1000, -1000}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 5,
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kSsat, 
            .reg_state1  = {GPR::kX1, 77777, 15}, 
            .reg_state2  = {GPR::kX2, 1000, 1000}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 5,
            .pc_initial  = 0x44,
            .pc_expected = 0x48
        },
        TestCpuState {
            .opcode      = Opcode::kAdd, 
            .reg_state1  = {GPR::kX0, 15, 15}, 
            .reg_state2  = {GPR::kX1, 20, 20}, 
            .reg_state3  = {GPR::kX2, 100, 35}, 
            .imm         = 0,
            .pc_initial  = 0x04,
            .pc_expected = 0x08
        },
        TestCpuState {
            .opcode      = Opcode::kAdd, 
            .reg_state1  = {GPR::kX0, -15, -15}, 
            .reg_state2  = {GPR::kX1, 20, 20}, 
            .reg_state3  = {GPR::kX2, 100, 5}, 
            .imm         = 0,
            .pc_initial  = 0x04,
            .pc_expected = 0x08
        },
        TestCpuState {
            .opcode      = Opcode::kCls, 
            .reg_state1  = {GPR::kX0, 77777, 31}, 
            .reg_state2  = {GPR::kX1, -2, -2}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 0,
            .pc_initial  = 0x04,
            .pc_expected = 0x08
        },
        TestCpuState {
            .opcode      = Opcode::kCls, 
            .reg_state1  = {GPR::kX0, 77777, 32}, 
            .reg_state2  = {GPR::kX1, -1, -1}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 0,
            .pc_initial  = 0x04,
            .pc_expected = 0x08
        },
        TestCpuState {
            .opcode      = Opcode::kCls, 
            .reg_state1  = {GPR::kX0, 77777, 0}, 
            .reg_state2  = {GPR::kX1, 2, 2}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 0,
            .pc_initial  = 0x04,
            .pc_expected = 0x08
        },
        TestCpuState {
            .opcode      = Opcode::kLi, 
            .reg_state1  = {GPR::kX0, 77777, -1}, 
            .reg_state2  = {GPR::kUnknown, 0, 0}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 0b1111'1111'1111'1111,
            .pc_initial  = 0x04,
            .pc_expected = 0x08
        },
        TestCpuState {
            .opcode      = Opcode::kLi, 
            .reg_state1  = {GPR::kX0, 77777, 1}, 
            .reg_state2  = {GPR::kUnknown, 0, 0}, 
            .reg_state3  = {GPR::kUnknown, 0, 0}, 
            .imm         = 1,
            .pc_initial  = 0x04,
            .pc_expected = 0x08
        }
    )
);

//--------------------------------------------------------------------------------

TEST(MemoryUsageExecutionTest, canExecuteLdAligned)
{
    Interpreter inter{};
    CpuState& cpu = inter.cpu_;
    RAM& ram = inter.ram_;

    cpu.SetProgramCounter (0x00);

    GPR base = GPR::kX0;
    GPR rt   = GPR::kX1;

    Word offset = -4;
    Word base_addr = 40;
    Word value = 55555;

    cpu.SetRegValue (base, base_addr);
    cpu.SetRegValue (rt,   77777);

    ram.StoreWord (base_addr + offset, value);

    Instruction instr = {
        .opcode_ = Opcode::kLd,
        .reg1_   = base,
        .reg2_   = rt,
        .imm_    = offset 
    };

    cpu.Execute (instr, ram);

    EXPECT_EQ (cpu.GetRegValue (base), base_addr);
    EXPECT_EQ (cpu.GetRegValue (rt), value);

    EXPECT_EQ (cpu.GetProgramCounter (), 0x04);
}

//--------------------------------------------------------------------------------

TEST(MemoryUsageExecutionTest, canThrowFromLdMisAligned)
{
    Interpreter inter{};
    CpuState& cpu = inter.cpu_;
    RAM& ram = inter.ram_;

    cpu.SetProgramCounter (0x00);

    GPR base = GPR::kX0;
    GPR rt   = GPR::kX1;

    Word offset = -2;
    Word base_addr = 40;
    Word value = 55555;

    cpu.SetRegValue (base, base_addr);
    cpu.SetRegValue (rt,   77777);

    Instruction instr = {
        .opcode_ = Opcode::kLd,
        .reg1_   = base,
        .reg2_   = rt,
        .imm_    = offset 
    };

    EXPECT_THROW(cpu.Execute (instr, ram), std::runtime_error);
}

//--------------------------------------------------------------------------------

TEST(MemoryUsageExecutionTest, canExecuteStAligned)
{ 
    Interpreter inter{};
    CpuState& cpu = inter.cpu_;
    RAM& ram = inter.ram_;

    cpu.SetProgramCounter (0x00);

    GPR base = GPR::kX0;
    GPR rt   = GPR::kX1;

    Word offset = -4;
    Word base_addr = 40;
    Word value = 55555;

    cpu.SetRegValue (base, base_addr);
    cpu.SetRegValue (rt,   value);

    Instruction instr = {
        .opcode_ = Opcode::kSt,
        .reg1_   = base,
        .reg2_   = rt,
        .imm_    = offset 
    };

    cpu.Execute (instr, ram);

    EXPECT_EQ (cpu.GetRegValue (base), base_addr);
    EXPECT_EQ (cpu.GetRegValue (rt), value);

    EXPECT_EQ (cpu.GetProgramCounter (), 0x04);

    EXPECT_EQ (ram.LoadWord (base_addr + offset), value);
}

//--------------------------------------------------------------------------------

TEST(MemoryUsageExecutionTest, canThrowFromStMisAligned)
{
    Interpreter inter{};
    CpuState& cpu = inter.cpu_;
    RAM& ram = inter.ram_;

    cpu.SetProgramCounter (0x00);

    GPR base = GPR::kX0;
    GPR rt   = GPR::kX1;

    Word offset = -2;
    Word base_addr = 40;
    Word value = 55555;

    cpu.SetRegValue (base, base_addr);
    cpu.SetRegValue (rt,   77777);

    Instruction instr = {
        .opcode_ = Opcode::kSt,
        .reg1_   = base,
        .reg2_   = rt,
        .imm_    = offset 
    };

    EXPECT_THROW(cpu.Execute (instr, ram), std::runtime_error);
}

//--------------------------------------------------------------------------------

TEST(MemoryUsageExecutionTest, canExecuteStpAligned)
{
    Interpreter inter{};
    CpuState& cpu = inter.cpu_;
    RAM& ram = inter.ram_;

    cpu.SetProgramCounter (0x00);

    GPR base = GPR::kX0;
    GPR rt1  = GPR::kX1;
    GPR rt2  = GPR::kX2;

    Word offset = -4;
    Word base_addr = 40;

    Word value1 = 11111;
    Word value2 = 22222;

    cpu.SetRegValue (base, base_addr);
    cpu.SetRegValue (rt1,  value1);
    cpu.SetRegValue (rt2,  value2);

    Instruction instr = {
        .opcode_ = Opcode::kStp,
        .reg1_   = base,
        .reg2_   = rt1,
        .reg3_   = rt2,
        .imm_    = offset 
    };

    cpu.Execute (instr, ram);

    EXPECT_EQ (cpu.GetRegValue (base), base_addr);
    EXPECT_EQ (cpu.GetRegValue (rt1), value1);
    EXPECT_EQ (cpu.GetRegValue (rt2), value2);

    EXPECT_EQ (cpu.GetProgramCounter (), 0x04);

    EXPECT_EQ (ram.LoadWord (base_addr + offset), value1);
    EXPECT_EQ (ram.LoadWord (base_addr + offset + 4), value2);
}

//--------------------------------------------------------------------------------

TEST(MemoryUsageExecutionTest, canThrowFromStpMisAligned)
{
    Interpreter inter{};
    CpuState& cpu = inter.cpu_;
    RAM& ram = inter.ram_;

    cpu.SetProgramCounter (0x00);

    GPR base = GPR::kX0;
    GPR rt1  = GPR::kX1;
    GPR rt2  = GPR::kX2;

    Word offset = -4;
    Word base_addr = 41;

    Word value1 = 11111;
    Word value2 = 22222;

    cpu.SetRegValue (base, base_addr);
    cpu.SetRegValue (rt1,  value1);
    cpu.SetRegValue (rt2,  value2);

    Instruction instr = {
        .opcode_ = Opcode::kStp,
        .reg1_   = base,
        .reg2_   = rt1,
        .reg3_   = rt2,
        .imm_    = offset 
    };

    EXPECT_THROW(cpu.Execute (instr, ram), std::runtime_error);
}

//--------------------------------------------------------------------------------

TEST(SyscallExecutionTest, canExecuteSyscallExit)
{
    Interpreter inter{};
    CpuState& cpu = inter.cpu_;
    RAM& ram = inter.ram_;

    cpu.SetProgramCounter (0x00);

    cpu.SetRegValue (GPR::kX0, EXIT_SUCCESS);
    cpu.SetRegValue (GPR::kX8, kSyscallCodeExit);

    Instruction instr = {
        .opcode_ = Opcode::kSyscall
    };

    EXPECT_THROW(cpu.Execute (instr, ram), GuestExit);
}

//--------------------------------------------------------------------------------

TEST(SyscallExecutionTest, canExecuteSyscallWrite)
{
    Interpreter inter{};
    CpuState& cpu = inter.cpu_;
    RAM& ram = inter.ram_;

    cpu.SetProgramCounter (0x00);

    cpu.SetRegValue (GPR::kX1, 77777);
    cpu.SetRegValue (GPR::kX8, kSyscallCodeWrite);

    Instruction instr = {
        .opcode_ = Opcode::kSyscall
    };

    testing::internal::CaptureStdout();

    cpu.Execute (instr, ram);

    std::string expected = "77777\n";

    EXPECT_EQ (expected, ::testing::internal::GetCapturedStdout ());
}

//--------------------------------------------------------------------------------
