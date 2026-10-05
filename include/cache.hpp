#pragma once

//————————————————————————————————————————————————————————————————————————————————

#include "decode.hpp"
#include <vector>
#include <unordered_map>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

using BasicBlock = std::vector<Instruction>;

//==================================================

constexpr PCValue kPCInvalidState = static_cast<PCValue>(-1);

//==================================================

class Cache
{
private:
    using BBCache = std::unordered_map<PCValue, BasicBlock>;

    BBCache map_;

    PCValue cur_bb_key_ = kPCInvalidState;

public:
    Cache () {}

    bool Prefetch (PCValue pc, BasicBlock& dest);

    void Add (Instruction& decoded_instr, PCValue pc);
};

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————