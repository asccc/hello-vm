#include "run.h"

/**
 * Intrinsic functions
 */

#define RUN_INTR static inline \
  __attribute__((always_inline, nonnull))

RUN_INTR intptr_t regr_a (struct vm *vm, 
                          struct vm_arg *arg)
{
  switch (arg->reg) {
    case REG_R0: return vm->r0;
    case REG_R1: return vm->r1;
    case REG_R2: return vm->r2;
    case REG_BP: return vm->bp;
    case REG_SP: return vm->sp;
    default:
      vm_exit(vm, VM_EUNKR);
      return 0;
  }
}

RUN_INTR void regw_a (struct vm *vm,
                      struct vm_arg *arg,
                      intptr_t ip)
{
  switch (arg->reg) {
    case REG_R0: vm->r0 = ip; break;
    case REG_R1: vm->r1 = ip; break;
    case REG_R2: vm->r2 = ip; break;
    case REG_BP: vm->bp = ip; break;
    case REG_SP: vm->sp = ip; break;
    default:
      vm_exit(vm, VM_EUNKR);
  }
}

/**
 * Memory access
 */

static void memw_8 (struct vm *vm, intptr_t ptr, u8 val)
{
  if (ptr < vm->mn || ptr > vm->mx) {
    vm_exit(vm, VM_EMBND);
    return;
  }
  *((u8*) ptr) = val;
}

static u8 memr_8 (struct vm *vm, intptr_t ptr)
{
  if (ptr < vm->mn || ptr > vm->mx) {
    vm_exit(vm, VM_EMBND);
    return 0;
  }
  return *(u8*) ptr;
}

/**
 * ModR/M API
 */

RUN_CALL void mrmw_8 (RUN_ARGS, u8 val)
{
  switch (arg->type) {
    case ARG_IRM:
      memw_8(vm, regr_a(vm, arg) + arg->off, val);
      break;
    case ARG_REG:
      regw_8(vm, arg, val);
      break;
    default:
      vm_exit(vm, VM_EINMR);
  }
}

RUN_CALL u8 mrmr_8 (RUN_ARGS)
{
  switch (arg->type) {
    case ARG_IRM:
      return memr_8(vm, regr_a(vm, arg) + arg->off);
    case ARG_REG:
      return regr_8(vm, arg);
    default:
      vm_exit(vm, VM_EINMR);
      return 0;
  }
}

/**
 * Register API
 */

RUN_CALL void regw_8 (RUN_ARGS, u8 val)
{
  switch (arg->reg) {
    case REG_R0:
      vm->r0 &= MSK_8;
      vm->r0 |= val;
      return;
    case REG_R1:
      vm->r1 &= MSK_8;
      vm->r1 |= val;
      return;
    case REG_R2:
      vm->r2 &= MSK_8;
      vm->r2 |= val;
      return;
  }
  if (sizeof(intptr_t) != 1) {
    vm_exit(vm, VM_EIPSZ);
    return;
  }
  switch (arg->reg) {
    case REG_BP: 
      vm->bp = val; 
      return;
    case REG_SP: 
      vm->sp = val;
      return;
  }
  vm_exit(vm, VM_EUNKR);
}

RUN_CALL u8 regr_8 (RUN_ARGS)
{
  switch (arg->reg) {
    case REG_R0: 
      return vm->r0;
    case REG_R1: 
      return vm->r1;
    case REG_R2: 
      return vm->r2;
  }
  if (sizeof(intptr_t) != 1) {
    vm_exit(vm, VM_EIPSZ);
    return 0;
  }
  switch (arg->reg) {
    case REG_BP: 
      return vm->bp;
    case REG_SP: 
      return vm->sp;
  }
  vm_exit(vm, VM_EUNKR);
  return 0;
}

/**
 * Immediate API
 */

RUN_CALL u8 immr_8 (struct vm *vm)
{
  u8 out = 0;
  vm_read(vm, 1, &out);
  return out;
}
