#include <cstdlib>
#include "decode.hpp"
#include "error_handle.hpp"
#include "cpu.hpp"
#include "logger.hpp"
#include "interpreter.hpp"
#include "memory.hpp"
#include "cache.hpp"

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

void CpuState::ExecuteBasicBlock (BasicBlock block, RAM& ram)
{
    LOG_TRACE_("Executing BasicBlock with size = {}", block.size ());

    for (auto instr : block) {
        LOG_TRACE_("BB executing {}", GetStringOpcode (instr.opcode_));
    
        Execute (instr, ram);
    }

    LOG_TRACE_("END of Executing BasicBlock");
}

//————————————————————————————————————————————————————————————————————————————————

int ExecuteProgram (BinaryCode& bin_code)
{
    Interpreter interpreter{};

    try
    {
    while (true) {
        BasicBlock block{};

        PCValue pc = interpreter.cpu_.GetProgramCounter ();

        LOG_TRACE_("Trying to prefetch at {}", pc);

        if (interpreter.cache_.Prefetch (pc, block)) {
            LOG_TRACE_("Prefetching Succeeded");

            interpreter.cpu_.ExecuteBasicBlock (block, interpreter.ram_);

            continue;
        }

        LOG_TRACE_("Prefetching Failed");
        LOG_TRACE_("Fetching instruction...");

        Word encoding = interpreter.cpu_.Fetch (bin_code);

        LOG_TRACE_("Fetched {:032b} ({:08X})", encoding, encoding);
        LOG_TRACE_("Decoding instruction...");

        Instruction instr = interpreter.cpu_.Decode (encoding);

        interpreter.cache_.Add (instr, pc);

        interpreter.cpu_.Execute (instr, interpreter.ram_);
    }
    } 
    catch (const GuestExit& exit)
    {
        LOG_TRACE_("Exiting normally... ");

        return exit.GetExitCode ();    
    }

    return EXIT_SUCCESS;
}

//————————————————————————————————————————————————————————————————————————————————

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————