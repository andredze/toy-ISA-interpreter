#pragma once

//--------------------------------------------------------------------------------

#include "cpu.hpp"
#include "memory.hpp"

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

class Interpreter
{
public:
    CpuState cpu_;
    RAM      ram_;
    Cache    cache_;

    Word Fetch ()
    {
        return ram_.LoadWord (cpu_.GetProgramCounter ());
    }
};

int ExecuteProgram (const std::string& file_path);

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------