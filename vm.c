#include "vm.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

/* lazy typedefs */
typedef struct vm_arg vm_arg;
typedef struct vm_val vm_val;
typedef struct vm_stk vm_stk;
typedef struct vm_str vm_str;

#if 0
/* push argument to stack */
static VM_INTR push_arg (struct vm *, struct vm_arg *);
/* push value to stack */
static VM_INTR push_val (struct vm *, struct vm_val *);
/* pop stack to argument */
static VM_INTR pop_arg (struct vm *, struct vm_arg *);
/* pop stack to value */
static VM_INTR pop_val (struct vm *, struct vm_val *);
#endif

/* copy arguments */
static VM_INTR copy_arg (struct vm *, vm_arg *, vm_arg *);
/* copy values */
static VM_INTR copy_val (struct vm *, vm_val *, vm_val *);
/* calls a argument symbol */ 
static VM_INTR call_arg (struct vm *, vm_arg *);

/* initializes a stack */
static void stk_init (struct vm *, vm_stk *);
/* releases a stack */
static void stk_free (struct vm *, vm_stk *);

/* initialize a value */
static VM_INTR val_init (struct vm *, vm_val *);
/* releases a value */
static VM_INTR val_free (struct vm *, vm_val *);

/**
 * {@inheritdoc}
 */
VM_CALL void vm_init (struct vm *vm)
{
  assert(vm != 0);
  vm->ep = 0;
  val_init(vm, &(vm->r0));
  val_init(vm, &(vm->r1));
  vm->sp = &(vm->bp);
  stk_init(vm, vm->sp);
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
    switch (op->kind) {
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
  return true;
}

/* --------------------------------- */

static void stk_init (struct vm *vm, struct vm_stk *stk)
{
  stk->ep = 0;
  stk->size = 0;
  stk->buff = 0;
  stk->argc = 0;
  stk->argv = NULL;
  stk->data = NULL;
  stk->prev = NULL;
  stk->next = NULL;
}

static void stk_free (struct vm *vm, vm_stk *stk)
{
  // noop
}

/* --------------------------------- */

static VM_INTR val_init (struct vm *vm, vm_val *val)
{
  memset(val, 0, sizeof(vm_val));
  val->type = VAR_NIL;
}

static VM_INTR val_free (struct vm *vm, vm_val *val)
{
  if (val->intr) {
    /* value is interned */
    return;
  }

  if (val->type == VAR_STR) {
    free(val->data.str.data);
  }

  val_init(vm, val);
}

/* --------------------------------- */

static VM_INTR call_arg (struct vm *vm, vm_arg *arg)
{
  if (arg->data.val.type != VAR_FNC) {
    puts("not a function");
    abort();
    return;
  }

  (arg->data.val.data.fnc)(vm);
}

/* --------------------------------- */

static VM_INTR copy_arg (struct vm *vm, vm_arg *dst, vm_arg *src)
{
  vm_val *dst_val = 0;
  vm_val *src_val = 0;

  switch (src->type) {
    case OPT_REG:
      /* read from register */
      switch (src->data.reg) {
        case REG_R0:
          src_val = &(vm->r0);
          break;
        case REG_R1:
          src_val = &(vm->r1);
          break;
        default:
          assert(0);
      }
      break;
    case OPT_VAL:
      /* read from value */
      src_val = &(src->data.val);
      break;
    default:
      assert(0);
  }

  switch (dst->type) {
    case OPT_REG:
      /* write to register */
      switch (dst->data.reg) {
        case REG_R0:
          dst_val = &(vm->r0);
          break;
        case REG_R1:
          dst_val = &(vm->r1);
          break;
        default:
          assert(0);
      }
      break;
    case OPT_VAL:
      /* write to value */
      dst_val = &(dst->data.val);
      break;
    default:
      assert(0);
  }

  if (dst_val == src_val) {
    // read and write to same value
    return;
  }

  copy_val(vm, dst_val, src_val);
}

static VM_INTR copy_num (struct vm *vm, vm_val *dst, vm_val *src)
{
  assert(src->type == VAR_NUM);
  dst->type = VAR_NUM;
  dst->data.num = src->data.num;
}

static VM_INTR copy_str (struct vm *vm, vm_val *dst, vm_val *src)
{
  assert(src->type == VAR_STR);

  vm_str *ds = &(dst->data.str);
  vm_str *ss = &(src->data.str);

  ds->size = ss->size;
  ds->buff = 0;
  ds->data = malloc(ss->size);

  if (!ds->data) {
    puts("out of memory");
    abort();
    return;
  }

  memcpy(ds->data, ss->data,  ss->size);
  dst->type = VAR_STR;
}

static VM_INTR copy_fnc (struct vm *vm, vm_val *dst, vm_val *src)
{
  assert(src->type == VAR_FNC);
  dst->type = VAR_FNC;
  dst->data.fnc = src->data.fnc;
}

static VM_INTR copy_val (struct vm *vm, vm_val *dst, vm_val *src)
{
  if (dst->intr) {
    puts("write to read-only value");
    abort();
  }

  /* clear destination */
  val_free(vm, dst);

  switch (src->type) {
    case VAR_NUM:
      copy_num(vm, dst, src);
      break;
    case VAR_STR:
      copy_str(vm, dst, src);
      break;
    case VAR_FNC:
      copy_fnc(vm, dst, src);
      break;
    case VAR_NIL:
      break;
    default:
      assert(0);
  }
}
