#include "bb_cache.hpp"
#include <cassert>

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

bool BBCache::Lookup (PCValue pc, BasicBlock& dest)
{
    auto it = map_.find (pc);

    if (it == map_.end ()) {
        // no basic block by pc
        return false;
    }

    dest = it->second;

    return true;
}

//--------------------------------------------------------------------------------

void BBCache::Add (Instruction& decoded_instr, PCValue pc)
{
    assert (map_.find (pc) == map_.end ());

    if (cur_bb_key_ == kPCInvalidState) {
        cur_bb_key_ = pc;
    }

    BasicBlock& block = map_[cur_bb_key_];

    block.instrs_.push_back (decoded_instr);

    if (ChangesControlFlow (decoded_instr.opcode_)) {
        block.exec_count_ = 0;
        cur_bb_key_ = kPCInvalidState;
    }
}

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------
