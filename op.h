#pragma once

#include "vm.h"

#define OP_CALL __attribute__((nonnull))
#define OP_FUNC OP_CALL enum op_res
#define OP_ARGS struct vm *vm, struct vm_opc *op, struct vm_imm *imm

enum op_res {
  RES_ERR = 0, /* error */
  RES_HLT,     /* halt */
  RES_CNT,     /* continue */
  RES_NXT      /* increment PC and continue */
};

extern enum op_res op_add_m8 (OP_ARGS);
extern enum op_res op_add_m16 (OP_ARGS);
extern enum op_res op_add_m32 (OP_ARGS);
extern enum op_res op_add_m64 (OP_ARGS);

extern enum op_res op_sub_m8 (OP_ARGS);
extern enum op_res op_sub_m16 (OP_ARGS);
extern enum op_res op_sub_m32 (OP_ARGS);
extern enum op_res op_sub_m64 (OP_ARGS);
