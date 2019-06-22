#include "../op.h"
#include "../run.h"

#define BINOP(F,S,O)             \
  static u ## S F ## _u ## S (   \
    struct vm *vm,               \
    u ## S a,                    \
    u ## S b                     \
  ) {                            \
    u ## S r = a O b;            \
    vm_flag(vm, FLG_ZF, r == 0); \
    return r;                    \
  }

#define BINOP_DIV(S)             \
  static u ## S div_u ## S (     \
    struct vm *vm,               \
    u ## S a,                    \
    u ## S b                     \
  ) {                            \
    if (b == 0) {                \
      vm_exit(vm, VM_EDIV0);     \
      return 0;                  \
    }                            \
    u ## S r = a / b;            \
    vm_flag(vm, FLG_ZF, r == 0); \
    return r;                    \
  }

#define OP_RM_R(S,F) do {             \
  mrmw_ ## S (vm, opc->args + 0,      \
    F ## _u ## S (vm,                 \
      mrmr_ ## S (vm, opc->args + 0), \
      regr_ ## S (vm, opc->args + 1)  \
    )                                 \
  );                                  \
} while (0)

#define OP_RM_IMM(S,F) do {           \
  mrmw_ ## S (vm, opc->args + 0,      \
    F ## _u ## S (vm,                 \
      mrmr_ ## S (vm, opc->args + 0), \
      immr_ ## S (vm)                 \
    )                                 \
  );                                  \
} while (0)

#define OP_R_RM(S,F) do {             \
  regw_ ## S (vm, opc->args + 0,      \
    F ## _u ## S (vm,                 \
      regr_ ## S (vm, opc->args + 0), \
      mrmr_ ## S (vm, opc->args + 1)  \
    )                                 \
  );                                  \
} while (0)

BINOP(add, 8, +)
BINOP(add, 16, +)
BINOP(add, 32, +)

OP_CALL op_add_rm8_r8 (OP_ARGS) { OP_RM_R(8, add); }
OP_CALL op_add_rm8_imm8 (OP_ARGS) { OP_RM_IMM(8, add); }
OP_CALL op_add_r8_rm8 (OP_ARGS) { OP_R_RM(8, add); }
OP_CALL op_add_rm16_r16 (OP_ARGS) { OP_RM_R(16, add); }
OP_CALL op_add_rm16_imm16 (OP_ARGS) { OP_RM_IMM(16, add); }
OP_CALL op_add_r16_rm16 (OP_ARGS) { OP_R_RM(16, add); }
OP_CALL op_add_rm32_r32 (OP_ARGS) { OP_RM_R(32, add); }
OP_CALL op_add_rm32_imm32 (OP_ARGS) { OP_RM_IMM(32, add); }
OP_CALL op_add_r32_rm32 (OP_ARGS) { OP_R_RM(32, add); }

#if VM_HAS_QWORD
  BINOP(add, 64, +)

  OP_CALL op_add_rm64_r64 (OP_ARGS) { OP_RM_R(64, add); }
  OP_CALL op_add_rm64_imm64 (OP_ARGS) { OP_RM_IMM(64, add); }
  OP_CALL op_add_r64_rm64 (OP_ARGS) { OP_R_RM(64, add); }
#endif

BINOP(sub, 8, -)
BINOP(sub, 16, -)
BINOP(sub, 32, -)

OP_CALL op_sub_rm8_r8 (OP_ARGS) { OP_RM_R(8, sub); }
OP_CALL op_sub_rm8_imm8 (OP_ARGS) { OP_RM_IMM(8, sub); }
OP_CALL op_sub_r8_rm8 (OP_ARGS) { OP_R_RM(8, sub); }
OP_CALL op_sub_rm16_r16 (OP_ARGS) { OP_RM_R(16, sub); }
OP_CALL op_sub_rm16_imm16 (OP_ARGS) { OP_RM_IMM(16, sub); }
OP_CALL op_sub_r16_rm16 (OP_ARGS) { OP_R_RM(16, sub); }
OP_CALL op_sub_rm32_r32 (OP_ARGS) { OP_RM_R(32, sub); }
OP_CALL op_sub_rm32_imm32 (OP_ARGS) { OP_RM_IMM(32, sub); }
OP_CALL op_sub_r32_rm32 (OP_ARGS) { OP_R_RM(32, sub); }

#if VM_HAS_QWORD
  BINOP(sub, 64, -)

  OP_CALL op_sub_rm64_r64 (OP_ARGS) { OP_RM_R(64, sub); }
  OP_CALL op_sub_rm64_imm64 (OP_ARGS) { OP_RM_IMM(64, sub); }
  OP_CALL op_sub_r64_rm64 (OP_ARGS) { OP_R_RM(64, sub); }
#endif

BINOP(mul, 32, *)

OP_CALL op_mul_rm32_r32 (OP_ARGS) { OP_RM_R(32, mul); }
OP_CALL op_mul_rm32_imm32 (OP_ARGS) { OP_RM_IMM(32, mul); }
OP_CALL op_mul_r32_rm32 (OP_ARGS) { OP_R_RM(32, mul); }

#if VM_HAS_QWORD
  BINOP(mul, 64, *)

  OP_CALL op_mul_rm64_r64 (OP_ARGS) { OP_RM_R(64, mul); }
  OP_CALL op_mul_rm64_imm64 (OP_ARGS) { OP_RM_IMM(64, mul); }
  OP_CALL op_mul_r64_rm64 (OP_ARGS) { OP_R_RM(64, mul); }
#endif

BINOP_DIV(32)

OP_CALL op_div_rm32_r32 (OP_ARGS) { OP_RM_R(32, div); }
OP_CALL op_div_rm32_imm32 (OP_ARGS) { OP_RM_IMM(32, div); }
OP_CALL op_div_r32_rm32 (OP_ARGS) { OP_R_RM(32, div); }

#if VM_HAS_QWORD
  BINOP_DIV(64)

  OP_CALL op_div_rm64_r64 (OP_ARGS) { OP_RM_R(64, div); }
  OP_CALL op_div_rm64_imm64 (OP_ARGS) { OP_RM_IMM(64, div); }
  OP_CALL op_div_r64_rm64 (OP_ARGS) { OP_R_RM(64, div); }
#endif
