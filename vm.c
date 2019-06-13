#include "vm.h"
#include "op.h"
#include "mem.h"

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

/**
 * {@inheritdoc}
 */
VM_CALL void vm_init (struct vm *vm)
{
  assert(vm != 0);

  vm->mem = mem_aodt(VM_STACK_SIZE);
  vm->sp = vm->mem;
  vm->bp = vm->mem;
  vm->ep = 0;
  vm->st = 0;

  memset(&(vm->r0), 0, sizeof(vm->r0));
  memset(&(vm->r1), 0, sizeof(vm->r1));
  memset(&(vm->r2), 0, sizeof(vm->r2));
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_flag (struct vm *vm, u32 flag)
{
  vm->st |= flag;
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_warn (struct vm *vm, const char *msg)
{
  perror(msg);
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_free (struct vm *vm)
{
  assert(vm != 0);

  mem_free(vm->mem);
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_exec (struct vm *vm, struct vm_op *ops)
{
  assert(vm != 0);
  assert(ops != 0);

  enum op_res rs;

  vm->ep = ops;
  for (struct vm_op *op;;) {
    op = vm->ep;
    switch (op->code) {
      #include "vm.inc"
    }
    switch (rs) {
      case RES_NXT:
        vm->ep++;
        /* fall through */
      case RES_CNT:
        break;
      case RES_ERR:
        puts("a error occurred");
        /* fall through */
      case RES_HLT:
        puts("execution halted");
        /* fall through */
      case RES_END:
        goto end;
      default:
        assert(0);
    }
  }
  end:
  return;
}

/**
 * {@inheritdoc}
 */
VM_CALL bool vm_args (struct vm *vm, const char *fmt, ...)
{
  assert(vm != 0);
  assert(fmt != 0);

  return false;
}
