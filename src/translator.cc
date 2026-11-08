#include "translator.hpp"
#include "interpreter.hpp"
#include "decode.hpp"
#include <ir_builder.h>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

void GenerateAdd (Interpreter& interp, ir_ctx* ctx, Instruction instr)
{
    assert (instr.opcode_ == Opcode::kAdd);

    ir_START();

    ir_ref x_addr   = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref y_addr   = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref res_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg3_));

    ir_ref x = ir_LOAD_U32 (x_addr);
    ir_ref y = ir_LOAD_U32 (y_addr);

    x = ir_ADD_U32 (x, y);

    ir_STORE (res_addr, x);

    ir_RETURN (IR_UNUSED);
}

//————————————————————————————————————————————————————————————————————————————————

} // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————
