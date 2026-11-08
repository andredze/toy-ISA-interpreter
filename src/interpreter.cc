#include <cstdlib>
#include "decode.hpp"
#include "../ir/ir.h"
#include "cpu.hpp"
#include "logger.hpp"
#include "interpreter.hpp"
#include "translator.hpp"
#include "elf_loader.hpp"
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

int ExecuteProgram (const std::string& file_path)
{
    Interpreter interpreter{};

    Word entry_addr{};

    if (!ELF::LoadFile (file_path, interpreter.ram_, &entry_addr)) {
        return EXIT_FAILURE;
    }

    interpreter.cpu_.SetProgramCounter (entry_addr);

    ir_ctx ctx;

    ir_consistency_check ();

    ir_init (&ctx, IR_FUNCTION | IR_OPT_FOLDING,
             IR_CONSTS_LIMIT_MIN, IR_INSNS_LIMIT_MIN);

    try
    {
    while (true) {
        LOG_TRACE_("Translating ADD");

        interpreter.cpu_.SetRegValue (GPR::kX0, 10);
        interpreter.cpu_.SetRegValue (GPR::kX1, 20);
        interpreter.cpu_.SetRegValue (GPR::kX2, 5);

        GenerateAdd (interpreter, &ctx, {
            .opcode_ = Opcode::kAdd,
            .reg1_ = GPR::kX0,
            .reg2_ = GPR::kX1,
            .reg3_ = GPR::kX2
            });

        std::size_t size{};

        void* entry{} = ir_jit_compile (&ctx, kOptimizationLevel, &size);

        if (entry) {
            reinterpret_cast<FuncExecuteTranslatedInstr> (entry) ();
        }

        LOG_TRACE_("Result of ADD\n"
                    "X0 = {}\nX1 = {}\nX2 = {}\n",
                    interpreter.cpu_.GetRegValue(GPR::kX0),
                    interpreter.cpu_.GetRegValue(GPR::kX1),
                    interpreter.cpu_.GetRegValue(GPR::kX2));

#if 0
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

        Word encoding = interpreter.Fetch ();

        LOG_TRACE_("Fetched {:032b} ({:08X})", encoding, encoding);
        LOG_TRACE_("Decoding instruction...");

        Instruction instr = interpreter.cpu_.Decode (encoding);

        interpreter.cache_.Add (instr, pc);

        interpreter.cpu_.Execute (instr, interpreter.ram_);
#endif
    }
    }
    catch (const GuestExit& exit)
    {
        LOG_TRACE_("Exiting normally... ");

        ir_free (&ctx);

        return exit.GetExitCode ();
    }

    ir_free (&ctx);

    return EXIT_SUCCESS;
}

//————————————————————————————————————————————————————————————————————————————————

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————
