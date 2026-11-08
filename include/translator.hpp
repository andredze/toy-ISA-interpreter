#pragma once

//————————————————————————————————————————————————————————————————————————————————

#include "interpreter.hpp"
#include <ir.h>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

using FuncExecuteTranslatedInstr = void (*) ();

void GenerateAdd (Interpreter& interp, ir_ctx* ctx, Instruction instr);
// Translate ();

//————————————————————————————————————————————————————————————————————————————————

} // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————
