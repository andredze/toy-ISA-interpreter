#include "translator.hpp"
#include "decode.hpp"
#include "bt_cache.hpp"
#include "interpreter.hpp"
#include <ir_builder.h>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

static void GenerateInstr (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc);

//————————————————————————————————————————————————————————————————————————————————

static void GenerateBasicBlock (Interpreter& interp, ir_ctx* ctx, BasicBlock& bb_block, PCValue pc)
{
    assert (ctx);

    ir_START ();

    if (bb_block.instrs_.empty ()) {
        ir_RETURN (IR_UNUSED);
        return;
    }

    std::size_t size = bb_block.instrs_.size ();

    for (std::size_t i = 0; i < size - 1; ++i) {
        GenerateInstr (interp, ctx, bb_block.instrs_[i], pc);
    }

    std::size_t translated_instrs_count = size - 1;

    ir_ref pc_addr = ir_CONST_ADDR (interp.cpu_.GetProgramCounterPointer ());

    Word advanced_pc = pc + translated_instrs_count * sizeof (Word);

    LOG_DEBUG_("PC before = {}\nPC after = {}", pc, advanced_pc);

    ir_STORE (pc_addr, ir_CONST_U32 (advanced_pc));

    // handle last instruction that changes control flow
    GenerateInstr (interp, ctx, bb_block.instrs_[size - 1], advanced_pc);

    ir_RETURN (IR_UNUSED);
}

//====================================

BlockTranslated TranslateBasicBlock (Interpreter& interp, BasicBlock& basic_block, PCValue pc)
{
    ir_ctx ctx;

    ir_consistency_check ();

    ir_init (&ctx, IR_FUNCTION | IR_OPT_FOLDING,
             IR_CONSTS_LIMIT_MIN, IR_INSNS_LIMIT_MIN);

    try
    {
        GenerateBasicBlock (interp, &ctx, basic_block, pc);
    }
    catch (const std::exception& except)
    {
        ir_free (&ctx);
        throw;
    }

    std::size_t code_size;

    void* code_entry = ir_jit_compile (&ctx, kOptimizationLevel, &code_size);

#ifdef LOGGING
    std::string block_name   = std::format ("{}{}", kDotFileName, interp.bt_cache_.GetSize ());
    std::string dot_filepath = std::format ("{}/{}.dot", kDotDir, block_name);
    std::string svg_filepath = std::format ("{}/{}.svg", kSvgDir, block_name);

    FILE* file = fopen (dot_filepath.c_str (), "w");

    if (file) {
        ir_dump_dot (&ctx, block_name.c_str (), nullptr, file);
        fclose (file);
    }

    std::string command = std::format ("dot -Tsvg {} -o {}", dot_filepath, svg_filepath);
    std::system (command.c_str ());
#endif

    ir_free (&ctx);

    BlockTranslated block{reinterpret_cast<TranslatedFunc>(code_entry), code_size};

    return block;
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateBeq (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kBeq);

    ir_ref r1_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref r2_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref pc_addr = ir_CONST_ADDR (interp.cpu_.GetProgramCounterPointer ());

    ir_ref val1 = ir_LOAD_U32 (r1_addr);
    ir_ref val2 = ir_LOAD_U32 (r2_addr);

    Word if_true_pc  = pc + (
        SignExtend (instr.imm_, GetFieldWidth (kBeqOffsetFieldLocation)) << 2
    );

    Word if_false_pc = pc + sizeof (Word);

    ir_ref cond = ir_IF (ir_EQ (val1, val2));

    ir_IF_TRUE (cond);
        ir_STORE (pc_addr, ir_CONST_U32 (if_true_pc));
        ir_ref end1 = ir_END();

    ir_IF_FALSE (cond);
        ir_STORE (pc_addr, ir_CONST_U32 (if_false_pc));
        ir_ref end2 = ir_END();

    ir_MERGE_2 (end1, end2);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateJ (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kJ);

    ir_ref pc_addr = ir_CONST_ADDR (interp.cpu_.GetProgramCounterPointer ());

    Word new_pc = (pc & 0xF000'0000) | (instr.imm_ << 2);

    ir_STORE (pc_addr, ir_CONST_U32 (new_pc));
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateRori (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kRori);

    ir_ref src_addr  = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref dest_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref imm       = ir_CONST_U32  (
        SignExtend (instr.imm_, GetFieldWidth (kAddiImmediateFieldLocation))
    );

    ir_ref src = ir_LOAD_U32 (src_addr);

    src = ir_ROR_U32 (src, imm);

    ir_STORE (dest_addr, src);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateAddi (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kAddi);

    ir_ref src_addr  = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref dest_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref imm       = ir_CONST_U32  (
        SignExtend (instr.imm_, GetFieldWidth (kAddiImmediateFieldLocation))
    );

    ir_ref src = ir_LOAD_U32 (src_addr);

    src = ir_ADD_U32 (src, imm);

    ir_STORE (dest_addr, src);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateXor (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kXor);

    ir_ref x_addr   = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref y_addr   = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref res_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg3_));

    ir_ref x = ir_LOAD_U32 (x_addr);
    ir_ref y = ir_LOAD_U32 (y_addr);

    x = ir_XOR_U32 (x, y);

    ir_STORE (res_addr, x);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateMovn (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kMovn);

    ir_ref cond_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref src_addr  = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref dest_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg3_));

    ir_ref cond_val = ir_LOAD_U32 (cond_addr);

    ir_ref cond = ir_IF (ir_NE (cond_val, ir_CONST_U32 (0)));

    ir_IF_TRUE (cond);
        ir_ref src = ir_LOAD_U32 (src_addr);
        ir_STORE (dest_addr, src);
        ir_ref end1 = ir_END ();

    ir_IF_FALSE (cond);
        ir_ref end2 = ir_END ();

    ir_MERGE_2 (end1, end2);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateAdd (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kAdd);

    ir_ref x_addr   = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref y_addr   = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref res_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg3_));

    ir_ref x = ir_LOAD_U32 (x_addr);
    ir_ref y = ir_LOAD_U32 (y_addr);

    x = ir_ADD_U32 (x, y);

    ir_STORE (res_addr, x);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateCls (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kCls);

    ir_ref src_addr  = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref dest_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));

    ir_ref val = ir_LOAD_U32 (src_addr);
    ir_ref res = ir_CTLZ_U32 (ir_NOT_U32 (val));

    ir_STORE (dest_addr, res);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateLi (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kLi);

    ir_ref dest_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));
    ir_ref imm       = ir_CONST_U32  (instr.imm_);

    ir_STORE (dest_addr, imm);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateInstr (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    LOG_TRACE_("Translating instr {}", GetStringOpcode (instr.opcode_));

    switch (instr.opcode_)
    {
#if 0
    case Opcode::kSyscall: GenerateSyscall (interp, ctx, instr, pc); break;
    case Opcode::kBext:    GenerateBext    (interp, ctx, instr, pc); break;
    case Opcode::kLd:      GenerateLd      (interp, ctx, instr, pc); break;
    case Opcode::kSt:      GenerateSt      (interp, ctx, instr, pc); break;
#endif
    case Opcode::kBeq:     GenerateBeq     (interp, ctx, instr, pc); break;
    case Opcode::kJ:       GenerateJ       (interp, ctx, instr, pc); break;
    case Opcode::kRori:    GenerateRori    (interp, ctx, instr, pc); break;
    case Opcode::kAddi:    GenerateAddi    (interp, ctx, instr, pc); break;
#if 0
    case Opcode::kStp:     GenerateStp     (interp, ctx, instr, pc); break;
#endif
    case Opcode::kXor:     GenerateXor     (interp, ctx, instr, pc); break;
    case Opcode::kMovn:    GenerateMovn    (interp, ctx, instr, pc); break;
#if 0
    case Opcode::kSsat:    GenerateSsat    (interp, ctx, instr, pc); break;
#endif
    case Opcode::kAdd:     GenerateAdd     (interp, ctx, instr, pc); break;
    case Opcode::kCls:     GenerateCls     (interp, ctx, instr, pc); break;
    case Opcode::kLi:      GenerateLi      (interp, ctx, instr, pc); break;
    case Opcode::kUnknown:
    default:
        throw std::runtime_error ("Failed to generate: Unknown instruction opcode");
    }
}

//————————————————————————————————————————————————————————————————————————————————

} // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————
