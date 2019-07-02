#pragma once

#include "vm.h"

#define OP_CALL __attribute__((nonnull, noinline)) void
#define OP_ARGS struct vm *vm, struct vm_ins *ins

#if VM64
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
