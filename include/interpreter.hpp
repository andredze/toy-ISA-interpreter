#pragma once

//--------------------------------------------------------------------------------

#include "cpu.hpp"

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

class Interpreter
{
public:
    CpuState cpu_;
    RAM      ram_;
    Cache    cache_;
};

int ExecuteProgram (BinaryCode& bin_code);

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------