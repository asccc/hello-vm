#include "vm.h"
#include "mem.h"
#include "run.h"

#include "op.h"

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#define READ_ARGS     \
  struct vm *vm,      \
  struct vm_ins *ins

#define EVAL_ARGS     \
  struct vm *vm,      \
  struct vm_ins *ins

#define EVAL_PASS vm, ins

/** reads raw memory */
static bool read_mem (struct vm*, szt, void*);
/** reads a opcode */
static bool read_opc (READ_ARGS);

/** checks the opcode before evaluation */
static bool chck_opc (EVAL_ARGS);

/** evaluates a opcode */
static void eval_opc (EVAL_ARGS);

#define DEFN_OP(N,H,S) do { \
  vm->oph[N] = H;           \
  vm->ops[N] = S;           \
} while (0)

/**
 * {@inheritdoc}
 */
VM_CALL void vm_init (struct vm *vm)
{
  assert(vm != 0);
  memset(vm->oph, 0, NUM_OPS * sizeof(vm_oph));
  memset(vm->ops, 0, NUM_OPS * sizeof(vm_ops));

  // initialize context
  vm->ip = 0;
  vm->ep = 0;
  vm->err = 0;
  vm->hlt = 0;

  vm->flg.cf = 0;
  vm->flg.of = 0;
  vm->flg.sf = 0;
  vm->flg.zf = 0;

  // general purpose registers
  vm->r0 = 1;
  vm->r1 = 0;
  vm->r2 = 0;
  
  // initialize stack
  vm->mm = mem_aodt(64);
  vm->sp = (intptr_t) (vm->mm + 64);
  vm->bp = vm->sp;

  // memory boundaries
  vm->lb = (intptr_t) (vm->mm);
  vm->ub = (intptr_t) (vm->mm + 64);

  // built-in ops (not dispatched)
  vm->oph[OPC_NOP] = 0;
  vm->oph[OPC_HLT] = 0;

  #include "vm.tab"
}

VM_CALL void vm_exit (struct vm *vm, enum vm_err err)
{
  vm->hlt = true;
  vm->err = err;

#ifndef NDEBUG
  if (err > VM_EHLT) {
    printf("ERROR: %u\n", err);
  }
#endif
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_warn (struct vm *vm, const char *msg)
{
  fputs("warning: ", stderr);
  fputs(msg, stderr);
  fputs("\n", stderr);
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_free (struct vm *vm)
{
  assert(vm != 0);
  // clear program
  vm->ip = 0;
  vm->ep = 0;
  // release memory
  mem_free(vm->mm);
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_exec (struct vm *vm, u8 *ops, szt len)
{
  assert(vm != 0);
  assert(ops != 0);

  struct vm_ins ins;
  
  vm->ip = ops;
  vm->ep = ops + len;

  while (vm->ip < vm->ep) {
    memset(&ins, 0, sizeof(ins));

    if (!read_ins(vm, &ins)) {
      vm_exit(vm, VM_EINS);
      goto end;
    }

    if (!chck_ins(vm, &ins)) {
      vm_exit(vm, VM_ECHK);
      goto end;
    }
    
    eval_ins(vm, &ins);

    if (vm->hlt) {
      // vm_exit() called somewhere
      goto end;
    }
  }

  end:
  return;
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_read (struct vm *vm, szt s, void *o)
{
  read_mem(vm, s, o);
}

/** reads raw memory */
static bool read_mem (struct vm *vm, szt sz, void *out)
{
  memcpy(out, vm->ip, sz);
  vm->ip += sz;
  return true;
}

/** reads a opcode */
static bool read_opc (READ_ARGS)
{
  u32 bin; // opcode
  
  if (!read_mem(vm, 4, &bin)) {
    // read error, abort
    return false;
  }

  // 3 bits = mode
  ins->mode = (bin >> 29) & 0x7;
  // 12 bits = code
  ins->code = (bin >> 17) & 0x3fff;

  struct vm_arg 
    *a0 = ins->args + 0,
    *a1 = ins->args + 1;

  // 3 bits = arg 0 type
  a0->type = (bin >> 14) & 0x7;
  // 5 bits = arg 0 reg
  a0->reg = (bin >> 9) & 0x1f;

  // 3 bits = arg 1 type
  a1->type = (bin >> 6) & 0x7;
  // 5 bits = arg 1 reg
  a1->reg = (bin >> 1) & 0x1f;

  // 1 bit = displacement
  ins->dsp = (bin & 1);

  return true;
}

/** checks the opcode before evaluation */
static bool chck_opc (EVAL_ARGS)
{
  if (ins->args[0].type == ARG_IRM &&
      ins->args[1].type == ARG_IRM) {
    vm_warn(vm, "only one argument can "
                "use memory-addressing");
    return false;
  }
  // all checks passed
  return true;
}

/** evaluates a opcode */
static void eval_opc (EVAL_ARGS)
{
  if (ins->code == OPC_NOP) {
    return;
  }

  if (ins->code == OPC_HLT) {
    vm_exit(vm, VM_EHLT);
    return;
  }

  vm_oph oph = vm->oph[ins->code];
  
  if (oph == 0) {
    vm_exit(vm, VM_EOPH);
    return;
  }

  #ifndef NDEBUG
    vm_ops ops = vm->ops[ins->code];
    printf(
      "OPCODE: %s (0x%x)\n", 
      ops ? ops : "(builtin)",
      ins->code
    );
  #endif

  (oph)(EVAL_PASS);
}
