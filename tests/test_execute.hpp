#pragma once

//--------------------------------------------------------------------------------

#include "cpu.hpp"
#include <gtest/gtest.h>

//--------------------------------------------------------------------------------

TEST(Executer, Add)
{
    CpuState cpu{};
    RAM ram{};

    GPRValue val1 = 110;
    GPRValue val2 = 13;

    cpu.SetRegValue (GPR::kX0, 10);
    cpu.SetRegValue (GPR::kX1, val1); 
    cpu.SetRegValue (GPR::kX2, val2); 

    Instruction instr = {
        .opcode_ = Opcode::kAdd,
        .reg1_   = GPR::kX1,
        .reg2_   = GPR::kX2,
        .reg3_   = GPR::kX0
    };

    cpu.Execute (instr, ram);

    EXPECT_EQ (cpu.GetRegValue (GPR::kX0), val1 + val2);
    EXPECT_EQ (cpu.GetRegValue (GPR::kX1), val1);
    EXPECT_EQ (cpu.GetRegValue (GPR::kX2), val2);
}

//--------------------------------------------------------------------------------

TEST(Executer, Add)
{
    CpuState cpu{};
    RAM ram{};

    GPRValue val1 = 110;
    GPRValue val2 = 13;

    cpu.SetRegValue (GPR::kX0, 10);
    cpu.SetRegValue (GPR::kX1, val1); 
    cpu.SetRegValue (GPR::kX2, val2); 

    Instruction instr = {
        .opcode_ = Opcode::kAdd,
        .reg1_   = GPR::kX1,
        .reg2_   = GPR::kX2,
        .reg3_   = GPR::kX0
    };

    cpu.Execute (instr, ram);

    EXPECT_EQ (cpu.GetRegValue (GPR::kX0), val1 + val2);
    EXPECT_EQ (cpu.GetRegValue (GPR::kX1), val1);
    EXPECT_EQ (cpu.GetRegValue (GPR::kX2), val2);
}

//--------------------------------------------------------------------------------