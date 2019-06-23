#include "../op.h"
#include "../run.h"

#define PUSH_R(S) do {               \
  vm->sp -= (S / 8u);                \
  memw_ ## S (vm, vm->sp,            \
    regr_ ## S (vm, opc->args + 0)); \
} while (0)

#define PUSH_RM(S) do {              \
  vm->sp -= (S / 8u);                \
  memw_ ## S (vm, vm->sp,            \
    mrmr_ ## S (vm, opc->args + 0)); \
} while (0)

#define PUSH_IMM(S) do {             \
  vm->sp -= (S / 8u);                \
  memw_8(vm, vm->sp, immr_8(vm));    \
} while (0)

OP_CALL op_push_r8 (OP_ARGS) { PUSH_R(8); }
OP_CALL op_push_rm8 (OP_ARGS) { PUSH_RM(8); }
OP_CALL op_push_imm8 (OP_ARGS) { PUSH_IMM(8); }
OP_CALL op_push_r16 (OP_ARGS) { PUSH_R(16); }
OP_CALL op_push_rm16 (OP_ARGS) { PUSH_RM(16); }
OP_CALL op_push_imm16 (OP_ARGS) { PUSH_IMM(16); }
OP_CALL op_push_r32 (OP_ARGS) { PUSH_R(32); }
OP_CALL op_push_rm32 (OP_ARGS) { PUSH_RM(32); }
OP_CALL op_push_imm32 (OP_ARGS) { PUSH_IMM(32); }

#if VM_HAS_QWORD
  OP_CALL op_push_r64 (OP_ARGS) { PUSH_R(64); }
  OP_CALL op_push_rm64 (OP_ARGS) { PUSH_RM(64); }
  OP_CALL op_push_imm64 (OP_ARGS) { PUSH_IMM(64); }
#endif

#define POP_R(S) do {            \
  regw_ ## S (vm, opc->args + 0, \
    memr_ ## S (vm, vm->sp));    \
  vm->sp += (S / 8u);            \
} while (0)

#define POP_RM(S) do {           \
  mrmw_ ## S (vm, opc->args + 0, \
    memr_ ## S (vm, vm->sp));    \
  vm->sp += (S / 8u);            \
} while (0)

OP_CALL op_pop_r8 (OP_ARGS) { POP_R(8); }
OP_CALL op_pop_rm8 (OP_ARGS) { POP_RM(8); }
OP_CALL op_pop_r16 (OP_ARGS) { POP_R(16); }
OP_CALL op_pop_rm16 (OP_ARGS) { POP_RM(16); }
OP_CALL op_pop_r32 (OP_ARGS) { POP_R(32); }
OP_CALL op_pop_rm32 (OP_ARGS) { POP_RM(32); }

#if VM_HAS_QWORD
  OP_CALL op_pop_r64 (OP_ARGS) { POP_R(64); }
  OP_CALL op_pop_rm64 (OP_ARGS) { POP_RM(64); }
#endif

#define MOV_RM_R(S) do {             \
  mrmw_ ## S (vm, opc->args + 0,     \
    regr_ ## S (vm, opc->args + 1)); \
} while (0)

#define MOV_RM_IMM(S) do {           \
  mrmw_ ## S (vm, opc->args + 0,     \
    immr_ ## S (vm));                \
} while (0)

#define MOV_R_RM(S) do {             \
  regw_ ## S (vm, opc->args + 0,     \
    mrmr_ ## S (vm, opc->args + 1)); \
} while (0)

OP_CALL op_mov_rm8_r8 (OP_ARGS) { MOV_RM_R(8); }
OP_CALL op_mov_rm8_imm8 (OP_ARGS) { MOV_RM_IMM(8); }
OP_CALL op_mov_r8_rm8 (OP_ARGS) { MOV_R_RM(8); }
OP_CALL op_mov_rm16_r16 (OP_ARGS) { MOV_RM_R(16); }
OP_CALL op_mov_rm16_imm16 (OP_ARGS) { MOV_RM_IMM(16); }
OP_CALL op_mov_r16_rm16 (OP_ARGS) { MOV_R_RM(16); }
OP_CALL op_mov_rm32_r32 (OP_ARGS) { MOV_RM_R(32); }
OP_CALL op_mov_rm32_imm32 (OP_ARGS) { MOV_RM_IMM(32); }
OP_CALL op_mov_r32_rm32 (OP_ARGS) { MOV_R_RM(8); }

#if VM_HAS_QWORD
  OP_CALL op_mov_rm64_r64 (OP_ARGS) { MOV_RM_R(64); }
  OP_CALL op_mov_rm64_imm64 (OP_ARGS) { MOV_RM_IMM(64); }
  OP_CALL op_mov_r64_rm64 (OP_ARGS) { MOV_R_RM(64); }
#endif
