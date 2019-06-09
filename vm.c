#include "vm.h"
#include "mem.h"

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

/** initializes a variable */
static VM_INTR var_init (struct vm*, struct vm_arg*, struct vm_arg*);

/** releases a variable */
static VM_INTR var_free (struct vm*, struct vm_arg*);

/** initializes a function call */
static VM_INTR fnc_init (struct vm*, struct vm_arg*);

/** pushes a value onto the argument stack */
static VM_INTR fnc_send (struct vm*, struct vm_arg*);

/** calls the initialized function */
static VM_INTR fnc_call (struct vm *);

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
      #include "vm.inc"
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
