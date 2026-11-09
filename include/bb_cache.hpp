#pragma once

//————————————————————————————————————————————————————————————————————————————————

#include "decode.hpp"
#include <vector>
#include <unordered_map>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

struct BasicBlock
{
    std::vector<Instruction> instrs_{};
    int exec_count_{};
};

//==================================================

constexpr PCValue kPCInvalidState = static_cast<PCValue>(-1);

//==================================================

class BBCache
{
// FIXME: interpreter.cc: exec_count++ logic
// private:
public:
    using BBCacheMap = std::unordered_map<PCValue, BasicBlock>;

    BBCacheMap map_;

    PCValue cur_bb_key_ = kPCInvalidState;

public:
    bool Lookup (PCValue pc, BasicBlock& dest);

    void Add (Instruction& decoded_instr, PCValue pc);
};

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————
