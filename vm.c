#include "vm.h"

#include <string.h>
#include <assert.h>

#define NEXT vm->ep++; break;

static void push_arg (struct vm *, struct vm_arg *);
static void push_val (struct vm *, u64);
static void pop_arg (struct vm *, struct vm_arg *);
static void pop_val (struct vm *, u64 *);
static void mov_arg (struct vm *, struct vm_arg *, struct vm_arg *);
static void read_arg (struct vm *, struct vm_arg *, u64 *);
static void copy_arg (struct vm *, struct vm_arg *, u64);

VM_CALL void vm_init (struct vm *vm)
{
  assert(vm != 0);

  vm->sp = vm->bp = 15;
  vm->ep = vm->cx = 0;
  vm->r0 = vm->r1 = 0;

  memset(vm->stk, 0, 16);
}

VM_CALL void vm_exec (struct vm *vm, struct vm_op *ops)
{
  assert(vm != 0);
  assert(ops != 0);

  for (struct vm_op *op;;) {
    op = (ops + vm->ep);
    switch (op->kind) {
      case OPC_NOP: {
        NEXT
      }
      case OPC_PUSH: {
        push_arg(vm, op->args);
        NEXT
      }
      case OPC_POP: {
        pop_arg(vm, op->args);
        NEXT
      }
      case OPC_MOV: {
        mov_arg(vm, op->args, op->args + 1);
        NEXT
      }
      case OPC_CALL: {
        push_val(vm, vm->sp);
        push_val(vm, vm->bp);
        push_val(vm, vm->ep);
        read_arg(vm, op->args, &(vm->ep));
        vm->bp = vm->sp;
        vm->sp = 0;
        break;
      }
      case OPC_RET: {
        pop_val(vm, &(vm->ep));
        pop_val(vm, &(vm->bp));
        pop_val(vm, &(vm->sp));
        NEXT;
      }
      case OPC_JMP: {
        read_arg(vm, op->args, &(vm->ep));
        NEXT;
      }
      case OPC_CMP: {
        u64 a = 0;
        u64 b = 0;
        read_arg(vm, op->args + 0, &a);
        read_arg(vm, op->args + 1, &b);
        vm->cx = (a - b);
        NEXT;
      }
      case OPC_JE: {
        if (vm->cx == 0) {
          read_arg(vm, op->args, &(vm->ep));
        }
        NEXT;
      }
      case OPC_JL: {
        if (vm->cx < 0) {
          read_arg(vm, op->args, &(vm->ep));
        }
        NEXT;
      }
      case OPC_JG: {
        if (vm->cx > 0) {
          read_arg(vm, op->args, &(vm->ep));
        }
        NEXT;
      }
      case OPC_ADD: {
        u64 a = 0;
        u64 b = 0;
        read_arg(vm, op->args + 0, &a);
        read_arg(vm, op->args + 1, &b);
        copy_arg(vm, op->args, a + b);
        NEXT;
      }
      case OPC_SUB: {
        u64 a = 0;
        u64 b = 0;
        read_arg(vm, op->args + 0, &a);
        read_arg(vm, op->args + 1, &b);
        copy_arg(vm, op->args, a - b);
        NEXT;
      }
      case OPC_INC: {
        u64 a;
        read_arg(vm, op->args, &a);
        a += 1;
        copy_arg(vm, op->args, a);
        NEXT;
      }
      case OPC_DEC: {
        u64 a;
        read_arg(vm, op->args, &a);
        a -= 1;
        copy_arg(vm, op->args, a);
        NEXT;
      }
      case OPC_END: goto end;
    }
  }

  end:
  return;
}

#define STK_CHECK_OVERFLOW(i) { \
  if ((i) == VM_STK_MIN) {      \
    puts("stack overflow!");    \
    abort();                    \
    return;                     \
  }                             \
}

#define STK_CHECK_UNDERFLOW(i) { \
  if ((i) == VM_STK_MAX) {       \
    puts("stack underflow!");    \
    abort();                     \
    return;                      \
  }                              \
}

static void push_arg (struct vm *vm, struct vm_arg *arg)
{
  u64 idx = vm->bp + vm->sp;
  STK_CHECK_OVERFLOW(idx);
  read_arg(vm, arg, vm->stk[idx]);
  vm->sp -= 1;
}

static void push_val (struct vm *vm, u64 val)
{
  u64 idx = vm->bp + vm->sp;
  STK_CHECK_OVERFLOW(idx);
  vm->stk[idx] = val;
  vm->sp -= 1;
}

static void pop_arg (struct vm *vm, struct vm_arg *arg)
{
  u64 idx = vm->bp + vm->sp + 1;
  STK_CHECK_UNDERFLOW(idx);
  copy_arg(vm, arg, vm->stk[idx]);
  vm->sp += 1;
}

static void pop_val (struct vm *vm, u64 *val)
{
  u64 idx = vm->bp + vm->sp + 1;
  STK_CHECK_UNDERFLOW(idx);
  *val = vm->stk[idx];
  vm->sp += 1;
}

static void mov_arg(struct vm *vm, struct vm_arg *arg0, struct vm_arg *arg1)
{
  
}
