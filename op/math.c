#include "../op.h"
#include "../flg.h"
#include "../run.h"

#include <stdio.h>

#define MIN(a,b) ((a) > (b) ? (b) : (a))

#define BINOP_ADD(S)             \
  static u ## S add_u ## S (     \
    struct vm *vm,               \
    u ## S a,                    \
    u ## S b                     \
  ) {                            \
    u ## S r = a + b;            \
    calc_zf(vm, r);              \
    calc_cf(vm, r > MIN(a,b));   \
    calc_sf_ ## S (vm, r);       \
    calc_of_ ## S (vm, r, a, b); \
    return r;                    \
  }

#define BINOP_SUB(S)              \
  static u ## S sub_u ## S (      \
    struct vm *vm,                \
    u ## S a,                     \
    u ## S b                      \
  ) {                             \
    u ## S r = a - b;             \
    calc_zf(vm, r);               \
    calc_cf(vm, b > a);           \
    calc_sf_ ## S (vm, r);        \
    calc_of_ ## S (vm, r, a, -b); \
    return r;                     \
  }

// @TODO: set flags
#define BINOP_MUL(S)         \
  static u ## S mul_u ## S ( \
    struct vm *vm,           \
    u ## S a,                \
    u ## S b                 \
  ) {                        \
    u ## S r = a * b;        \
    calc_zf(vm, r == 0);     \
    return r;                \
  }

// @TODO: set flags
#define BINOP_DIV(S)         \
  static u ## S div_u ## S ( \
    struct vm *vm,           \
    u ## S a,                \
    u ## S b                 \
  ) {                        \
    if (b == 0) {            \
      vm_exit(vm, VM_EDIV0); \
      return 0;              \
    }                        \
    u ## S r = a / b;        \
    calc_zf(vm, r == 0);     \
    return r;                \
  }

#define OP_RM_R(S,F) do {             \
  mrmw_ ## S (vm, ins->args + 0,      \
    F ## _u ## S (vm,                 \
      mrmr_ ## S (vm, ins->args + 0), \
      regr_ ## S (vm, ins->args + 1)  \
    )                                 \
  );                                  \
} while (0)

#define OP_RM_IMM(S,F) do {           \
  mrmw_ ## S (vm, ins->args + 0,      \
    F ## _u ## S (vm,                 \
      mrmr_ ## S (vm, ins->args + 0), \
      immr_ ## S (vm)                 \
    )                                 \
  );                                  \
} while (0)

#define OP_R_RM(S,F) do {             \
  regw_ ## S (vm, ins->args + 0,      \
    F ## _u ## S (vm,                 \
      regr_ ## S (vm, ins->args + 0), \
      mrmr_ ## S (vm, ins->args + 1)  \
    )                                 \
  );                                  \
} while (0)

BINOP_ADD(8)
BINOP_ADD(32)

OP_CALL op_add_rm8_r8 (OP_ARGS) { OP_RM_R(8, add); }
OP_CALL op_add_rm8_imm8 (OP_ARGS) { OP_RM_IMM(8, add); }
OP_CALL op_add_r8_rm8 (OP_ARGS) { OP_R_RM(8, add); }
OP_CALL op_add_rm32_r32 (OP_ARGS) { OP_RM_R(32, add); }
OP_CALL op_add_rm32_imm32 (OP_ARGS) { OP_RM_IMM(32, add); }
OP_CALL op_add_r32_rm32 (OP_ARGS) { OP_R_RM(32, add); }

#if VM64
  BINOP_ADD(64)

  OP_CALL op_add_rm64_r64 (OP_ARGS) { OP_RM_R(64, add); }
  OP_CALL op_add_rm64_imm64 (OP_ARGS) { OP_RM_IMM(64, add); }
  OP_CALL op_add_r64_rm64 (OP_ARGS) { OP_R_RM(64, add); }
#endif

BINOP_SUB(8)
BINOP_SUB(32)

OP_CALL op_sub_rm8_r8 (OP_ARGS) { OP_RM_R(8, sub); }
OP_CALL op_sub_rm8_imm8 (OP_ARGS) { OP_RM_IMM(8, sub); }
OP_CALL op_sub_r8_rm8 (OP_ARGS) { OP_R_RM(8, sub); }
OP_CALL op_sub_rm32_r32 (OP_ARGS) { OP_RM_R(32, sub); }
OP_CALL op_sub_rm32_imm32 (OP_ARGS) { OP_RM_IMM(32, sub); }
OP_CALL op_sub_r32_rm32 (OP_ARGS) { OP_R_RM(32, sub); }

#if VM64
  BINOP_SUB(64)

  OP_CALL op_sub_rm64_r64 (OP_ARGS) { OP_RM_R(64, sub); }
  OP_CALL op_sub_rm64_imm64 (OP_ARGS) { OP_RM_IMM(64, sub); }
  OP_CALL op_sub_r64_rm64 (OP_ARGS) { OP_R_RM(64, sub); }
#endif

BINOP_MUL(32)

OP_CALL op_mul_rm32_r32 (OP_ARGS) { OP_RM_R(32, mul); }
OP_CALL op_mul_rm32_imm32 (OP_ARGS) { OP_RM_IMM(32, mul); }
OP_CALL op_mul_r32_rm32 (OP_ARGS) { OP_R_RM(32, mul); }

#if VM64
  BINOP_MUL(64)

  OP_CALL op_mul_rm64_r64 (OP_ARGS) { OP_RM_R(64, mul); }
  OP_CALL op_mul_rm64_imm64 (OP_ARGS) { OP_RM_IMM(64, mul); }
  OP_CALL op_mul_r64_rm64 (OP_ARGS) { OP_R_RM(64, mul); }
#endif

BINOP_DIV(32)

OP_CALL op_div_rm32_r32 (OP_ARGS) { OP_RM_R(32, div); }
OP_CALL op_div_rm32_imm32 (OP_ARGS) { OP_RM_IMM(32, div); }
OP_CALL op_div_r32_rm32 (OP_ARGS) { OP_R_RM(32, div); }

#if VM64
  BINOP_DIV(64)

  OP_CALL op_div_rm64_r64 (OP_ARGS) { OP_RM_R(64, div); }
  OP_CALL op_div_rm64_imm64 (OP_ARGS) { OP_RM_IMM(64, div); }
  OP_CALL op_div_r64_rm64 (OP_ARGS) { OP_R_RM(64, div); }
#endif

OP_CALL op_cmp_rm8_r8 (OP_ARGS) { OP_RM_R(8, sub); }
OP_CALL op_cmp_rm8_imm8 (OP_ARGS) { OP_RM_IMM(8, sub); }
OP_CALL op_cmp_r8_rm8 (OP_ARGS) { OP_R_RM(8, sub); }
OP_CALL op_cmp_rm32_r32 (OP_ARGS) { OP_RM_R(32, sub); }
OP_CALL op_cmp_rm32_imm32 (OP_ARGS) { OP_RM_IMM(32, sub); }
OP_CALL op_cmp_r32_rm32 (OP_ARGS) { OP_R_RM(32, sub); }

#if VM64
  OP_CALL op_cmp_rm64_r64 (OP_ARGS) { OP_RM_R(64, sub); }
  OP_CALL op_cmp_rm64_imm64 (OP_ARGS) { OP_RM_IMM(64, sub); }
  OP_CALL op_cmp_r64_rm64 (OP_ARGS) { OP_R_RM(64, sub); }
#endif
