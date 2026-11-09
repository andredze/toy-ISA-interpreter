#pragma once

//--------------------------------------------------------------------------------

#include "cpu.hpp"
#include "memory.hpp"
#include "bb_cache.hpp"
#include "bt_cache.hpp"

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//------------------------------------------------------------------------------

enum class TranslatedCodeStatus
{
    kOk,
    kMisalignedAccess,
    kSyscall
};

//--------------------------------------------------------------------------------

class Interpreter
{
public:
    CpuState cpu_;
    RAM      ram_;
    BBCache  bb_cache_;
    BTCache  bt_cache_;

    Word Fetch ()
    {
        return ram_.LoadWord (cpu_.GetProgramCounter ());
    }

    void CallTranslatedBlock (PCValue pc, BlockTranslated& bt)
    {
        TranslatedCodeStatus status = static_cast<TranslatedCodeStatus>(bt.entry_ ());

        if (status == TranslatedCodeStatus::kMisalignedAccess) {
            ram_.ThrowMisalignedAccess ();
        }
        else if (status == TranslatedCodeStatus::kSyscall) {
            LOG_TRACE_("Interpreting syscall");
            LOG_TRACE_("Fetching instruction...");

            Word encoding = Fetch ();

            LOG_TRACE_("Fetched {:032b} ({:08X})", encoding, encoding);
            LOG_TRACE_("Decoding instruction...");

            Instruction instr = cpu_.Decode (encoding);

            cpu_.Execute (instr, ram_);
        }
    }
};

int ExecuteProgram (const std::string& file_path);

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------
