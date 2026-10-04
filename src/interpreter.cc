#include "decode.hpp"
#include "error_handle.hpp"
#include "cpu.hpp"
#include "logger.hpp"
#include "interpreter.hpp"

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

int ExecuteProgram (BinaryCode& bin_code)
{
    CpuState cpu{};

    LOG_TRACE_("Fetching instruction...");

    while (true)
    {
        Word encoding = cpu.Fetch (bin_code);

        LOG_TRACE_("Fetched {:032b} ({:08X})", encoding, encoding);
        LOG_TRACE_("Decoding instruction...");

        Instruction instr = cpu.Decode (encoding);

        cpu.Execute (instr);
    }

    return 0;
}

//————————————————————————————————————————————————————————————————————————————————

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————