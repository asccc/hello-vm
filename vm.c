#include "vm.h"
#include "mem.h"

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#define VM_TEST                                 \
  __attribute__((always_inline, nonnull, pure)) \
  static inline void


/**
 * assertion handler:
 * 
 * tests if the given argument-count is correct
 * 
 * @param the opcode where this assertion was called
 * @param the assertion-result (true = passed, false = failed)
 * 
 */
VM_TEST test_argc (char *code, bool pass)
{
  if (!pass) {
    printf("ERR(%s): wrong number of arguments\n", code);
    abort();
  }
}

/**
 * assertion handler:
 * 
 * tests if the given argument-type is correct
 * 
 * @param the opcode where this assertion was called
 * @param the tested argument index 
 * @param the assertion-result (true = passed, false = failed)
 */
VM_TEST test_argv (char *code, u32 argi, bool pass)
{
  if (!pass) {
    printf("ERR(%s): invalid argument type passed #%u\n", code, argi);
    abort();
  }
}

/** initializes the stack */
static void init_stk (struct vm*, u32);

/** releases the current stack */
static void free_stk (struct vm*);

/** updates a value on the stack */
static void updt_var (struct vm*, u32, struct vm_arg*);

/** deletes a variable-value on the stack */
static void free_var (struct vm*, u32);

/** initializes a invocation */
static void init_inv (struct vm*, struct vm_arg*, i64);

/** sends a variable to the current invocation */
static void send_var (struct vm*, u32);

/** sends a temporary value to the current invocation */
static void send_val (struct vm*, struct vm_arg*);

/** executes the current invocation */
static void exec_inv (struct vm*);

/**
 * {@inheritdoc}
 */
VM_CALL void vm_init (struct vm *vm)
{
  assert(vm != 0);

  vm->ep = 0;
  vm->stk = 0;
  vm->inv = 0;
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

  u32 idx = 0;
  u32 len = vm->inv->argc;

  while (*fmt) {
    if (idx >= len) {
      puts("no more arguments");
      abort();
    }
    switch (*fmt) {
      case 's': 
        *(va_arg(va, char**)) = vm->inv->argv[idx]->data.str;
        break;
    }

    idx++;
    fmt++;
  }

  va_end(va);

  return true;
}

/* ---------------------------------------------------- */

/** initializes the stack */
static void init_stk (struct vm *vm, u32 num)
{
  struct vm_stk *stk;

  stk = mem_aodt(sizeof(*stk));
  stk->data = mem_aodt(sizeof(struct vm_val*) * num);
  stk->size = num;
  stk->prev = vm->stk;

  vm->stk = stk;
}

/** releases the current stack */
static void free_stk (struct vm *vm)
{
  struct vm_stk *stk;

  stk = vm->stk;
  for (u32 i = 0; i < stk->size; ++i) {
    free_var(vm, i);
  }

  vm->stk = stk->prev;
  mem_free(stk->data);
  mem_free(stk);
}

/** updates a value on the stack */
static void updt_var (struct vm *vm, u32 vid, struct vm_arg *val)
{
  struct vm_val **var;

  if (vid >= vm->stk->size) {
    printf("variable id #%u out of bounds\n", vid);
    abort();
  }

  var = &(vm->stk->data[vid]);
  if (!*var) {
    *var = mem_aodt(sizeof(*var));
    (*var)->type = VAR_NIL;
    (*var)->temp = false;
  } else {
    if ((*var)->type == VAR_STR) {
      mem_free((*var)->data.str);
    }
  }

  switch (val->type) {
    case OPT_NUM:
      (*var)->type = VAR_NUM;
      (*var)->data.num = val->data.num;
      break;
    case OPT_STR:
      (*var)->type = VAR_STR;
      (*var)->data.str = mem_sdup(val->data.str);
      break;
    case OPT_SYM:
      (*var)->type = VAR_SYM;
      (*var)->data.sym = val->data.sym;
      break;
    default:
      assert(0);
  }
}

/** deletes a variable-value on the stack */
static void free_var (struct vm *vm, u32 vid)
{
  struct vm_stk *stk;
  struct vm_val **var;

  stk = vm->stk;
  if (vid >= stk->size) {
    printf("variable id #%u out of bounds\n", vid);
    abort();
  }

  var = &(stk->data[vid]);
  if (*var) {
    if ((*var)->type == VAR_STR) {
      mem_free((*var)->data.str);
    }
    mem_free(*var);
    *var = 0;
  }
}

/** initializes a invocation */
static void init_inv (struct vm *vm, struct vm_arg *val, i64 num)
{
  struct vm_inv *inv;

  if (vm->inv) {
    puts("there is already a invocation active");
    abort();
  }

  inv = mem_aodt(sizeof(*inv));
  inv->argc = num;
  inv->indx = 0;

  switch (val->type) {
    case OPT_VID: {
      struct vm_val *var;
      u32 vid = val->data.vid;
      if (vid >= vm->stk->size) {
        printf("variable id #%u out of bounds\n", vid);
        abort();
      }
      var = vm->stk->data[vid];
      if (!var || val->type != OPT_SYM) {
        puts("invalid value passed to invocation");
        abort();
      }
      inv->sym = var->data.sym;
      break;
    }
    case OPT_SYM: 
      inv->sym = val->data.sym;
      break;
    default:
      assert(0);
  }

  inv->argv = mem_aodt(sizeof(struct vm_val) * num);
  inv->retv = mem_aodt(sizeof(struct vm_val));

  vm->inv = inv;
}

/** sends a variable to the current invocation */
static void send_var (struct vm *vm, u32 vid)
{
  struct vm_inv *inv;
  struct vm_val **var;
  struct vm_val *dup;

  inv = vm->inv;
  if (!inv) {
    puts("no invocation active");
    abort();
  }

  if (inv->indx >= inv->argc) {
    puts("too many arguments");
    abort();
  }

  if (vid >= vm->stk->size) {
    printf("variable id #%u out of bounds\n", vid);
    abort();
  }

  var = &(vm->stk->data[vid]);
  if (!*var) {
    *var = mem_aodt(sizeof(**var));
    (*var)->type = VAR_NIL;
  }

  dup = mem_aodt(sizeof(*dup));
  switch ((*var)->type) {
    case VAR_NUM:
      dup->type = VAR_NUM;
      dup->data.num = (*var)->data.num;
      break;
    case VAR_STR:
      dup->type = VAR_STR;
      dup->data.str = mem_sdup((*var)->data.str);
      break;
    case VAR_SYM:
      dup->type = VAR_SYM;
      dup->data.sym = (*var)->data.sym;
      break;
    default:
      assert(0);
  }

  inv->argv[inv->indx++] = dup;
}

/** sends a temporary value to the current invocation */
static void send_val (struct vm *vm, struct vm_arg *val)
{
  struct vm_inv *inv;
  struct vm_val *tmp;

  inv = vm->inv;
  if (!inv) {
    puts("no invocation active");
    abort();
  }

  if (inv->indx >= inv->argc) {
    puts("too many arguments");
    abort();
  }

  tmp = mem_aodt(sizeof(*tmp));
  switch (val->type) {
    case OPT_NUM:
      tmp->type = VAR_NUM;
      tmp->data.num = val->data.num;
      break;
    case OPT_STR:
      tmp->type = VAR_STR;
      tmp->data.str = mem_sdup(val->data.str);
      break;
    case OPT_SYM:
      tmp->type = VAR_SYM;
      tmp->data.sym = val->data.sym;
      break;
    default:
      assert(0);
  }

  inv->argv[inv->indx++] = tmp;
}

/** executes the current invocation */
static void exec_inv (struct vm *vm)
{
  struct vm_inv *inv;
  struct vm_val *var;

  inv = vm->inv;
  if (!inv) {
    puts("no invocation active");
    abort();
  }

  (inv->sym)(vm);

  for (u32 i = 0; i < inv->argc; ++i) {
    var = inv->argv[i];
    if (var) {
      if (var->type == VAR_STR) {
        mem_free(var->data.str);
      }
      mem_free(var);
    }
  }

  mem_free(inv);
  vm->inv = 0;
}
