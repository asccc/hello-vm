#pragma once

#include "vm.h"

#define OP_CALL __attribute__((nonnull, noinline)) void
#define OP_ARGS struct vm *vm, struct vm_opc *opc

#if VM_HAS_QWORD
  #define OP_EX64 OP_CALL
#else
  #define OP_EX64 OP_CALL __attribute__((unused))
#endif

// --------------------------------
// defined in op/math.c

extern OP_CALL op_add_rm8_r8 (OP_ARGS);
extern OP_CALL op_add_rm8_imm8 (OP_ARGS);
extern OP_CALL op_add_r8_rm8 (OP_ARGS);
extern OP_CALL op_add_rm16_r16 (OP_ARGS);
extern OP_CALL op_add_rm16_imm16 (OP_ARGS);
extern OP_CALL op_add_r16_rm16 (OP_ARGS);
extern OP_CALL op_add_rm32_r32 (OP_ARGS);
extern OP_CALL op_add_rm32_imm32 (OP_ARGS);
extern OP_CALL op_add_r32_rm32 (OP_ARGS);
extern OP_EX64 op_add_rm64_r64 (OP_ARGS);
extern OP_EX64 op_add_rm64_imm64 (OP_ARGS);
extern OP_EX64 op_add_r64_rm64 (OP_ARGS);

extern OP_CALL op_sub_rm8_r8 (OP_ARGS);
extern OP_CALL op_sub_rm8_imm8 (OP_ARGS);
extern OP_CALL op_sub_r8_rm8 (OP_ARGS);
extern OP_CALL op_sub_rm16_r16 (OP_ARGS);
extern OP_CALL op_sub_rm16_imm16 (OP_ARGS);
extern OP_CALL op_sub_r16_rm16 (OP_ARGS);
extern OP_CALL op_sub_rm32_r32 (OP_ARGS);
extern OP_CALL op_sub_rm32_imm32 (OP_ARGS);
extern OP_CALL op_sub_r32_rm32 (OP_ARGS);
extern OP_EX64 op_sub_rm64_r64 (OP_ARGS);
extern OP_EX64 op_sub_rm64_imm64 (OP_ARGS);
extern OP_EX64 op_sub_r64_rm64 (OP_ARGS);

extern OP_CALL op_mul_rm32_r32 (OP_ARGS);
extern OP_CALL op_mul_rm32_imm32 (OP_ARGS);
extern OP_CALL op_mul_r32_rm32 (OP_ARGS);
extern OP_EX64 op_mul_rm64_r64 (OP_ARGS);
extern OP_EX64 op_mul_rm64_imm64 (OP_ARGS);
extern OP_EX64 op_mul_r64_rm64 (OP_ARGS);

extern OP_CALL op_div_rm32_r32 (OP_ARGS);
extern OP_CALL op_div_rm32_imm32 (OP_ARGS);
extern OP_CALL op_div_r32_rm32 (OP_ARGS);
extern OP_EX64 op_div_rm64_r64 (OP_ARGS);
extern OP_EX64 op_div_rm64_imm64 (OP_ARGS);
extern OP_EX64 op_div_r64_rm64 (OP_ARGS);

// --------------------------------
// defined in op/stor.c

extern OP_CALL op_push_r8 (OP_ARGS);
extern OP_CALL op_push_rm8 (OP_ARGS);
extern OP_CALL op_push_imm8 (OP_ARGS);
extern OP_CALL op_push_r16 (OP_ARGS);
extern OP_CALL op_push_rm16 (OP_ARGS);
extern OP_CALL op_push_imm16 (OP_ARGS);
extern OP_CALL op_push_r32 (OP_ARGS);
extern OP_CALL op_push_rm32 (OP_ARGS);
extern OP_CALL op_push_imm32 (OP_ARGS);
extern OP_EX64 op_push_r64 (OP_ARGS);
extern OP_EX64 op_push_rm64 (OP_ARGS);
extern OP_EX64 op_push_imm64 (OP_ARGS);

extern OP_CALL op_pop_r8 (OP_ARGS);
extern OP_CALL op_pop_rm8 (OP_ARGS);
extern OP_CALL op_pop_r16 (OP_ARGS);
extern OP_CALL op_pop_rm16 (OP_ARGS);
extern OP_CALL op_pop_r32 (OP_ARGS);
extern OP_CALL op_pop_rm32 (OP_ARGS);
extern OP_EX64 op_pop_r64 (OP_ARGS);
extern OP_EX64 op_pop_rm64 (OP_ARGS);

// extern OP_CALL op_cmp_rm8_r8 (OP_ARGS);
// extern OP_CALL op_cmp_rm8_imm8 (OP_ARGS);
// extern OP_CALL op_cmp_r8_rm8 (OP_ARGS);
// extern OP_CALL op_cmp_rm16_r16 (OP_ARGS);
// extern OP_CALL op_cmp_rm16_imm16 (OP_ARGS);
// extern OP_CALL op_cmp_r16_rm16 (OP_ARGS);
// extern OP_CALL op_cmp_rm32_r32 (OP_ARGS);
// extern OP_CALL op_cmp_rm32_imm32 (OP_ARGS);
// extern OP_CALL op_cmp_r32_rm32 (OP_ARGS);
// extern OP_EX64 op_cmp_rm64_r64 (OP_ARGS);
// extern OP_EX64 op_cmp_rm64_imm64 (OP_ARGS);
// extern OP_EX64 op_cmp_r64_rm64 (OP_ARGS);
 
// extern OP_CALL op_mov_rm8_r8 (OP_ARGS);
// extern OP_CALL op_mov_rm8_imm8 (OP_ARGS);
// extern OP_CALL op_mov_r8_rm8 (OP_ARGS);
// extern OP_CALL op_mov_rm16_r16 (OP_ARGS);
// extern OP_CALL op_mov_rm16_imm16 (OP_ARGS);
// extern OP_CALL op_mov_r16_rm16 (OP_ARGS);
// extern OP_CALL op_mov_rm32_r32 (OP_ARGS);
// extern OP_CALL op_mov_rm32_imm32 (OP_ARGS);
// extern OP_CALL op_mov_r32_rm32 (OP_ARGS);
// extern OP_EX64 op_mov_rm64_r64 (OP_ARGS);
// extern OP_EX64 op_mov_rm64_imm64 (OP_ARGS);
// extern OP_EX64 op_mov_r64_rm64 (OP_ARGS);

// --------------------------------
// defined in op/flow.c

// extern OP_CALL op_int (OP_ARGS);
// extern OP_CALL op_call (OP_ARGS);
// extern OP_CALL op_ret (OP_ARGS);
// extern OP_CALL op_jmp (OP_ARGS);
// extern OP_CALL op_jz (OP_ARGS);
// extern OP_CALL op_jl (OP_ARGS);
// extern OP_CALL op_jg (OP_ARGS);
