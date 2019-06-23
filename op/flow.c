#include "../op.h"
#include "../int.h"
#include "../run.h"

#define CHCK_JMP(O) do {         \
  if ((vm->ip + (O)) > vm->ep) { \
    vm_exit(vm, VM_EJMPB);       \
    return;                      \
  }                              \
} while (0)

OP_CALL op_int (OP_ARGS)
{
  u32 num = immr_32(vm);
  int_exec(vm, num);
}

OP_CALL op_call (OP_ARGS)
{
  // @TODO: define a dedicated stack api?
  // >> push pc
  // >> mov  pc, imm32
  u32 pc = immr_32(vm);
  CHCK_JMP(pc);
  vm->sp -= 4;
  memw_32(vm, vm->sp, vm->pc);
  vm->pc = pc;
}

OP_CALL op_ret (OP_ARGS)
{
  // @TODO: define a dedicated stack api?
  // >> pop  pc
  u32 pc = memr_32(vm, vm->sp);
  CHCK_JMP(pc);
  vm->pc = pc;
  vm->sp += 4;
}

OP_CALL op_jmp (OP_ARGS)
{
  u32 pc = immr_32(vm);
  CHCK_JMP(pc);
  vm->pc = pc;
}

OP_CALL op_jz (OP_ARGS)
{
  if (vm_fchk(vm, FLG_ZF)) {
    u32 pc = immr_32(vm);
    CHCK_JMP(pc);
    vm->pc = pc;
  }
}

OP_CALL op_jnz (OP_ARGS)
{
  if (!vm_fchk(vm, FLG_ZF)) {
    u32 pc = immr_32(vm);
    CHCK_JMP(pc);
    vm->pc = pc;
  }
}

OP_CALL op_jb (OP_ARGS)
{
  if (vm_fchk(vm, FLG_CF)) {
    u32 pc = immr_32(vm);
    CHCK_JMP(pc);
    vm->pc = pc;
  }
}

OP_CALL op_jnb (OP_ARGS)
{
  if (!vm_fchk(vm, FLG_CF)) {
    u32 pc = immr_32(vm);
    CHCK_JMP(pc);
    vm->pc = pc;
  }
}

OP_CALL op_jbe (OP_ARGS)
{
  if (vm_fchk(vm, FLG_CF) ||
      vm_fchk(vm, FLG_ZF)) {
    u32 pc = immr_32(vm);
    CHCK_JMP(pc);
    vm->pc = pc;
  }
}

OP_CALL op_ja (OP_ARGS)
{
  if (vm_fchk(vm, FLG_CF) &&
      vm_fchk(vm, FLG_ZF)) {
    u32 pc = immr_32(vm);
    CHCK_JMP(pc);
    vm->pc = pc;
  }
}
