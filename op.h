#pragma once

#include "vm.h"

#define OP_CALL __attribute__((nonnull))
#define OP_FUNC OP_CALL enum op_res
#define OP_ARGS struct vm *vm, struct vm_op *op

enum op_res {
  RES_ERR = 0, /* error */
  RES_HLT,     /* halt */
  RES_CNT,     /* continue */
  RES_NXT      /* increment PC and continue */
};
