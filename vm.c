#include "vm.h"

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

  vm->ep = 0;
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_exec (struct vm *vm, struct vm_op *ops)
{
  assert(vm != 0);
  assert(ops != 0);

  for (struct vm_op *op;;) {
    op = (ops + vm->ep);
    switch (op->code) {
      case OPC_NOP:
      case OPC_VAL:
      case OPC_SET:
      case OPC_INI:
      case OPC_SND:
      case OPC_EXC:
      case OPC_DEL:
        vm->ep++;
        break;
      case OPC_END:
        goto end;
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
  
  va_list va;
  va_start(va, fmt);

  va_end(va);

  return true;
}
