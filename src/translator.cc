#include "translator.hpp"
#include "interpreter.hpp"
#include "decode.hpp"
#include "../ir/ir.h"

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

void GenerateAdd (Interpreter& interp, ir_ctx* ctx, Instruction instr)
{
    assert (instr.opcode_ == Opcode::kAdd);

    ir_START();

#if 0
    ir_ref x_ptr = ir_PARAM (IR_I32, "x_ptr", interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref y_ptr = ir_PARAM (IR_I32, "y_ptr", interp.cpu_.GetRegPointer (instr.reg2_));
#endif

    ir_ref x = ir_LOAD_U32 (interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref y = ir_LOAD_U32 (interp.cpu_.GetRegPointer (instr.reg2_));

    ir_ref x = ir_ADD_U32 (x, y);

    Word res_ptr = interp.cpu_.GetRegPointer (instr.reg3_);

    ir_STORE (res_ptr, x);

    ir_RETURN (IR_UNUSED);
}

//————————————————————————————————————————————————————————————————————————————————

} // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————
