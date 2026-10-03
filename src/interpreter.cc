#include "decode.hpp"
#include "execute.hpp"
#include "error_handle.hpp"
#include "cpu.hpp"
#include "logger.hpp"

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

int ExecuteProgram (BinaryCode& bin_code)
{
    CpuState cpu{};

    LOG_TRACE_("Fetching instruction...");

    Word encoding = cpu.Fetch (bin_code);

    LOG_TRACE_("Fetched {:b} ({:#x})", encoding, encoding);
    LOG_TRACE_("Decoding instruction...");

    Instruction instr = Decode (encoding);

    // Execute (instr);

    return 0;
}

//————————————————————————————————————————————————————————————————————————————————

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————