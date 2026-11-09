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
    Instruction& last_instr = bb_block.instrs_[size - 1];

    GenerateInstr (interp, ctx, last_instr, advanced_pc);

    if (last_instr.opcode_ != Opcode::kSyscall) {
        ir_RETURN (ir_CONST_I32 (static_cast<int>(TranslatedCodeStatus::kOk)));
    }
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

static void GenerateSyscall (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kSyscall);

    ir_RETURN (ir_CONST_I32 (static_cast<int>(TranslatedCodeStatus::kSyscall)));
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateBext (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    // X[reg1] ← bit_extract(X[reg2], X[reg3])
    assert (ctx);
    assert (instr.opcode_ == Opcode::kBext);

    ir_ref mask_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg3_));
    ir_ref data_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref dest_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));

    ir_ref data = ir_LOAD_U32 (data_addr);
    ir_ref mask = ir_LOAD_U32 (mask_addr);

    ir_ref packed_bits = ir_VAR (IR_U32, "packed");

    for (int i = kWordSizeInBits - 1; i >= 0; --i) {
        // bit = (value >> i) & 1u);
        ir_ref data_bit = ir_AND_U32 (ir_SHR_U32 (data, ir_CONST_U32 (i)), ir_CONST_U32 (1u));
        ir_ref mask_bit = ir_AND_U32 (ir_SHR_U32 (mask, ir_CONST_U32 (i)), ir_CONST_U32 (1u));

        // packed = (packed << mask_bit) | (data_bit & mask_bit);
        // when mask_bit = 1, it shifts once and appends the result_bit
        // when mask_bit = 0, shift does not happen and the result_bit is 0

        ir_ref packed_bits = ir_OR_U32 (ir_SHL_U32 (packed_bits, mask_bit),
                                        ir_AND_U32 (data_bit, mask_bit));
    }

    ir_STORE (dest_addr, packed_bits);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateLd (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kLd);

    ir_ref base_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));

    ir_ref base = ir_LOAD_U32 (base_addr);
    ir_ref imm  = ir_CONST_U32 (SignExtend (instr.imm_, GetFieldWidth (kLdImmediateFieldLocation)));

    ir_ref addr = ir_ADD_U32 (base, imm);

    // if addr % sizeof (Word) == 0
    ir_ref last_bits = ir_SHL_U32 (addr, ir_CONST_U32 (kWordSizeInBits - kAlignmentZeroBitsCount));

    ir_ref cond = ir_IF (ir_EQ (last_bits, ir_CONST_U32 (0)));

    ir_IF_FALSE (cond);
        ir_RETURN (ir_CONST_I32 (static_cast<int>(TranslatedCodeStatus::kMisalignedAccess)));

    ir_IF_TRUE (cond);
        ir_ref mem_start_addr = ir_CONST_ADDR (interp.ram_.GetDataPointer ());
        ir_ref mem_src_addr   = ir_ADD_A (mem_start_addr, ir_ZEXT_A (addr));

        // FIXME: check bounds of memory
        ir_ref value = ir_LOAD_U32 (mem_src_addr);

        ir_ref dest_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
        ir_STORE (dest_addr, value);
}

//————————————————————————————————————————————————————————————————————————————————

static void GenerateSt (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kSt);

    ir_ref base_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));

    ir_ref base = ir_LOAD_U32 (base_addr);
    ir_ref imm  = ir_CONST_U32 (SignExtend (instr.imm_, GetFieldWidth (kLdImmediateFieldLocation)));

    ir_ref addr = ir_ADD_U32 (base, imm);

    // if addr % sizeof (Word) == 0
    ir_ref last_bits = ir_SHL_U32 (addr, ir_CONST_U32 (kWordSizeInBits - kAlignmentZeroBitsCount));

    ir_ref cond = ir_IF (ir_EQ (last_bits, ir_CONST_U32 (0)));

    ir_IF_FALSE (cond);
        ir_RETURN (ir_CONST_I32 (static_cast<int>(TranslatedCodeStatus::kMisalignedAccess)));

    ir_IF_TRUE (cond);
        ir_ref mem_start_addr = ir_CONST_ADDR (interp.ram_.GetDataPointer ());
        ir_ref mem_dest_addr  = ir_ADD_A (mem_start_addr, ir_ZEXT_A (addr));

        ir_ref src_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
        ir_ref value    = ir_LOAD_U32 (src_addr);

        // FIXME: check bounds of memory
        ir_STORE (mem_dest_addr, value);
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

static void GenerateStp (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kStp);

    ir_ref base_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));

    ir_ref base = ir_LOAD_U32 (base_addr);
    ir_ref imm  = ir_CONST_U32 (SignExtend (instr.imm_, GetFieldWidth (kLdImmediateFieldLocation)));

    ir_ref addr = ir_ADD_U32 (base, imm);

    // if addr % sizeof (Word) == 0
    ir_ref last_bits = ir_SHL_U32 (addr, ir_CONST_U32 (kWordSizeInBits - kAlignmentZeroBitsCount));

    ir_ref cond = ir_IF (ir_EQ (last_bits, ir_CONST_U32 (0)));

    ir_IF_FALSE (cond);
        ir_RETURN (ir_CONST_I32 (static_cast<int>(TranslatedCodeStatus::kMisalignedAccess)));

    ir_IF_TRUE (cond);
        ir_ref mem_start_addr = ir_CONST_ADDR (interp.ram_.GetDataPointer ());

        ir_ref mem_dest1_addr = ir_ADD_A (mem_start_addr, ir_ZEXT_A (addr));
        ir_ref mem_dest2_addr = ir_ADD_A (mem_dest1_addr, ir_CONST_ADDR (sizeof (Word)));

        ir_ref src1_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
        ir_ref value1 = ir_LOAD_U32 (src1_addr);

        // FIXME: check bounds of memory
        ir_STORE (mem_dest1_addr, value1);

        ir_ref src2_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg3_));
        ir_ref value2 = ir_LOAD_U32 (src2_addr);

        // FIXME: check bounds of memory
        ir_STORE (mem_dest2_addr, value2);
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

static void GenerateSsat (Interpreter& interp, ir_ctx* ctx, Instruction& instr, PCValue pc)
{
    assert (ctx);
    assert (instr.opcode_ == Opcode::kSsat);

    auto bits_count = instr.imm_;

    auto max_value = (1 << (bits_count - 1)) - 1;
    auto min_value = - (1 << (bits_count - 1));

    ir_ref src_addr  = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg2_));
    ir_ref dest_addr = ir_CONST_ADDR (interp.cpu_.GetRegPointer (instr.reg1_));

    ir_ref val = ir_LOAD_U32 (src_addr);

    val = ir_MAX_I32 (val, ir_CONST_I32 (max_value));
    val = ir_MIN_I32 (val, ir_CONST_I32 (min_value));

    ir_STORE (dest_addr, val);
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
    case Opcode::kSyscall: GenerateSyscall (interp, ctx, instr, pc); break;
    case Opcode::kBext:    GenerateBext    (interp, ctx, instr, pc); break;
    case Opcode::kLd:      GenerateLd      (interp, ctx, instr, pc); break;
    case Opcode::kSt:      GenerateSt      (interp, ctx, instr, pc); break;
    case Opcode::kBeq:     GenerateBeq     (interp, ctx, instr, pc); break;
    case Opcode::kJ:       GenerateJ       (interp, ctx, instr, pc); break;
    case Opcode::kRori:    GenerateRori    (interp, ctx, instr, pc); break;
    case Opcode::kAddi:    GenerateAddi    (interp, ctx, instr, pc); break;
    case Opcode::kStp:     GenerateStp     (interp, ctx, instr, pc); break;
    case Opcode::kXor:     GenerateXor     (interp, ctx, instr, pc); break;
    case Opcode::kMovn:    GenerateMovn    (interp, ctx, instr, pc); break;
    case Opcode::kSsat:    GenerateSsat    (interp, ctx, instr, pc); break;
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
