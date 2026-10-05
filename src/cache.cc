#include "cache.hpp"
#include <cassert>

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

bool Cache::Prefetch (PCValue pc, BasicBlock& dest)
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

void Cache::Add (Instruction decoded_instr, PCValue pc)
{
    assert (map_.find (pc) == map_.end ());

    if (cur_bb_key_ == kPCInvalidState) {
        cur_bb_key_ = pc;
    }

    map_[cur_bb_key_].push_back (decoded_instr);
    
    if (ChangesControlFlow (decoded_instr.opcode_)) {
        cur_bb_key_ = kPCInvalidState;
    }
}

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------