#pragma once

#include "vm.h"

#define OP_CALL __attribute__((nonnull, noinline)) void
#define OP_ARGS struct vm *vm, struct vm_opc *opc, struct vm_imm *imm

#if VM_USE_QWORD
  #define OP_CALL64 OP_CALL
#else
  #define OP_CALL64 OP_CALL __attribute__((unused))
#endif

// 8bit
extern OP_CALL op_add_m8 (OP_ARGS);
extern OP_CALL op_sub_m8 (OP_ARGS);

// 16bit
extern OP_CALL op_add_m16 (OP_ARGS);
extern OP_CALL op_sub_m16 (OP_ARGS);

// 32bit
extern OP_CALL op_add_m32 (OP_ARGS);
extern OP_CALL op_sub_m32 (OP_ARGS);

// 64bit
extern OP_CALL64 op_add_m64 (OP_ARGS);
extern OP_CALL64 op_sub_m64 (OP_ARGS);
