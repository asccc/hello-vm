#include "op.h"
#include "run.h"

static u8 add_u8 (struct vm*, u8, u8);

OP_CALL op_add_rm8_r8 (OP_ARGS)
{
  mrmw_8(vm, opc->args + 0,
    add_u8(vm,
      mrmr_8(vm, opc->args + 0),
      regr_8(vm, opc->args + 1)
    )
  );
}

OP_CALL op_add_rm8_imm8 (OP_ARGS)
{
  mrmw_8(vm, opc->args + 0,
    add_u8(vm,
      mrmr_8(vm, opc->args + 0),
      immr_8(vm)
    )
  );
}

OP_CALL op_add_r8_rm8 (OP_ARGS)
{
  regw_8(vm, opc->args + 0,
    add_u8(vm,
      regr_8(vm, opc->args + 0),
      mrmr_8(vm, opc->args + 1)
    )
  );
}

#define BINOP(N,O) do { \
  u ## N _r = a O b;    \
  if (_r == 0) {        \
    vm_flag(vm, FLG_Z); \
  }                     \
  return _r;            \
} while (0)

static u8 add_u8 (struct vm *vm, u8 a, u8 b)
{
  BINOP(8, +);
}
