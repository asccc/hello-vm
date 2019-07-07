#include "../op.h"
#include "../int.h"
#include "../run.h"

#include <stdio.h>

/** reads a displacement and jumps to it */
static void jump_dsp (struct vm*);
/** reads a displacement and discards it */
static void skip_dsp (struct vm*);

#define CHCK_JMP(O) do {         \
  if ((O) > (intptr_t) vm->ep || \
      (O) < (intptr_t) vm->pp) { \
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
  // >> push ip
  // >> mov  ip, dsp
  intptr_t ip = dspr_a(vm);
  CHCK_JMP(ip);
  vm->sp -= sizeof(ip);
  memw_a(vm, vm->sp, (intptr_t) vm->ip);
  vm->ip = (u8*) ip;
}

OP_CALL op_ret (OP_ARGS)
{
  // @TODO: define a dedicated stack api?
  // >> pop  ip
  intptr_t ip = memr_a(vm, vm->sp);
  CHCK_JMP(ip);
  vm->ip = (u8*) ip;
  vm->sp += sizeof(ip);
}

OP_CALL op_jmp (OP_ARGS)
{
  jump_dsp(vm);
}

OP_CALL op_jz (OP_ARGS)
{
  if (vm->flg.zf) {
    jump_dsp(vm);
    return;
  }
  skip_dsp(vm);
}

OP_CALL op_jnz (OP_ARGS)
{
  if (!vm->flg.zf) {
    jump_dsp(vm);
    return;
  }
  skip_dsp(vm);
}

OP_CALL op_jb (OP_ARGS)
{
  if (vm->flg.cf) {
    jump_dsp(vm);
    return;
  }
  skip_dsp(vm);
}

OP_CALL op_jnb (OP_ARGS)
{
  if (!vm->flg.cf) {
    jump_dsp(vm);
    return;
  }
  skip_dsp(vm);
}

OP_CALL op_jbe (OP_ARGS)
{
  if (vm->flg.cf || vm->flg.zf) {
    jump_dsp(vm);
    return;
  }
  skip_dsp(vm);
}

OP_CALL op_ja (OP_ARGS)
{
  if (vm->flg.cf && vm->flg.zf) {
    jump_dsp(vm);
    return;
  }
  skip_dsp(vm);
}

/** reads a displacement and jumps to it */
static void jump_dsp (struct vm *vm)
{
  intptr_t ip = dspr_a(vm);
  CHCK_JMP(ip);
  vm->ip = (u8*) ip;
}

/** reads a displacement and discards it */
static void skip_dsp (struct vm *vm)
{
  (void) dspr_a(vm);
}
