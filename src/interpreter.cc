#include <cstdlib>
#include "decode.hpp"
#include <ir.h>
#include "cpu.hpp"
#include "logger.hpp"
#include "interpreter.hpp"
#include "translator.hpp"
#include "elf_loader.hpp"
#include "memory.hpp"
#include "bb_cache.hpp"

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

void CpuState::ExecuteBasicBlock (BasicBlock block, RAM& ram)
{
    LOG_TRACE_("Executing BasicBlock with size = {}", block.instrs_.size ());

    for (auto instr : block.instrs_) {
        LOG_TRACE_("BB executing {}", GetStringOpcode (instr.opcode_));

        Execute (instr, ram);
    }

    LOG_TRACE_("END of Executing BasicBlock");
}

//————————————————————————————————————————————————————————————————————————————————

int ExecuteProgram (const std::string& file_path)
{
#ifdef LOGGING
    std::string command1 = std::format ("mkdir -p {}", kDotDir);
    std::string command2 = std::format ("mkdir -p {}", kSvgDir);

    system (command1.c_str ());
    system (command2.c_str ());
#endif

    Interpreter interpreter{};

    Word entry_addr{};

    if (!ELF::LoadFile (file_path, interpreter.ram_, &entry_addr)) {
        return EXIT_FAILURE;
    }

    interpreter.cpu_.SetProgramCounter (entry_addr);

    try
    {
    while (true) {
        BasicBlock      bb_block{};
        BlockTranslated bt_block{};

        PCValue pc = interpreter.cpu_.GetProgramCounter ();

        LOG_TRACE_("Trying to lookup BT at {}", pc);

        if (interpreter.bt_cache_.Lookup (pc, bt_block)) {
            LOG_TRACE_("Hit in BTCache");
            LOG_TRACE_("Executing BT block");

            interpreter.CallTranslatedBlock (pc, bt_block);

            continue;
        }
        LOG_TRACE_("No BT hit");

        LOG_TRACE_("Trying to lookup BB at {}", pc);

        if (interpreter.bb_cache_.Lookup (pc, bb_block)) {
            LOG_TRACE_("Hit in BBCache");
            LOG_TRACE_("BB block exec_count = {}", bb_block.exec_count_);

            if (bb_block.exec_count_ >= kBBTranslationThreshold) {
                LOG_TRACE_("BB block exec_count reached threshold {}, translating...",
                           kBBTranslationThreshold);

                bt_block = TranslateBasicBlock (interpreter, bb_block, pc);
                interpreter.bt_cache_.Add (pc, bt_block);

                LOG_TRACE_("Cached BT, executing");

                interpreter.CallTranslatedBlock (pc, bt_block);
            }
            else {
                LOG_TRACE_("Interpreting BB block");

                interpreter.cpu_.ExecuteBasicBlock (bb_block, interpreter.ram_);

                LOG_TRACE_("Incrementing exec_count {}",
                           interpreter.bb_cache_.map_[pc].exec_count_);
                // FIXME: not address cache second time
                interpreter.bb_cache_.map_[pc].exec_count_++;
            }

            continue;
        }
        LOG_TRACE_("No BB hit");
        LOG_TRACE_("Fetching instruction...");

        Word encoding = interpreter.Fetch ();

        LOG_TRACE_("Fetched {:032b} ({:08X})", encoding, encoding);
        LOG_TRACE_("Decoding instruction...");

        Instruction instr = interpreter.cpu_.Decode (encoding);

        interpreter.bb_cache_.Add (instr, pc);

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
