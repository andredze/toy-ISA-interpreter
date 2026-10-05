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
    CpuState cpu{};

    RAM ram{};

    Cache cache{};

    try
    {
    while (true) {
        BasicBlock block{};

        PCValue pc = cpu.GetProgramCounter ();

        LOG_TRACE_("Trying to prefetch at {}", pc);

        if (cache.Prefetch (pc, block)) {
            LOG_TRACE_("Prefetching Succeeded");

            cpu.ExecuteBasicBlock (block, ram);

            continue;
        }

        LOG_TRACE_("Prefetching Failed");
        LOG_TRACE_("Fetching instruction...");

        Word encoding = cpu.Fetch (bin_code);

        LOG_TRACE_("Fetched {:032b} ({:08X})", encoding, encoding);
        LOG_TRACE_("Decoding instruction...");

        Instruction instr = cpu.Decode (encoding);

        cache.Add (instr, pc);

        cpu.Execute (instr, ram);
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