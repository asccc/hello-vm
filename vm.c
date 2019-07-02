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
static bool read_ins (READ_ARGS);

/** check the bytecode mode */
static bool read_mod (struct vm*);

/** checks the opcode before evaluation */
static bool chck_ins (EVAL_ARGS);

/** evaluates a opcode */
static void eval_ins (EVAL_ARGS);

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
  vm->opm = MOD_NONE;

  vm->flg.cf = 0;
  vm->flg.of = 0;
  vm->flg.sf = 0;
  vm->flg.zf = 0;

  // general purpose registers
  vm->r0 = 0;
  vm->r1 = 0;
  vm->r2 = 0;
  
  // initialize stack
  vm->mm = mem_aodt(64);
  vm->sp = (intptr_t) (vm->mm + 64);
  vm->bp = vm->sp;

  // memory boundaries
  vm->mn = (intptr_t) (vm->mm);
  vm->mx = (intptr_t) (vm->mm + 64);

  // built-in ops (not dispatched)
  vm->oph[OPC_NOP] = 0;
  vm->ops[OPC_NOP] = "<nop>";
  vm->oph[OPC_HLT] = 0;
  vm->ops[OPC_HLT] = "<hlt>";

  #include "vm.inc"
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

  // read bytecode mode
  if (!read_mod(vm)) {
    vm_exit(vm, VM_EMOD);
    goto end;
  }

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

/** reads the bytecode mode */
static bool read_mod (struct vm *vm)
{
  i32 mod = 0;

  if (!read_mem(vm, sizeof(mod), &mod)) {
    // read error, abort
    return false;
  }

#ifndef NDEBUG
  printf("head:  %x\n", mod);
  printf("magic: %x\n", mod & 0x00ffffff);
  printf("mode:  %x\n", (mod >> 24) & 0xff);
#endif

  if ((mod & 0x00ffffff) != HVM_NUM) {
    vm_warn(vm, VM_ESTR_HVM);
    return false;
  }

  switch (mod = (mod >> 24) & 0xff) {
    case MOD_DWORD:
    case MOD_QWORD:
      break;
    default:
      vm_warn(vm, VM_ESTR_OPM);
      return false;
  }

  vm->opm = mod;
  return true;
}

/** reads a opcode */
static bool read_ins (READ_ARGS)
{
  u32 bin; // opcode
  
  if (!read_mem(vm, 4, &bin)) {
    // read error, abort
    return false;
  }

  // 3 bits = mode
  ins->mode = (bin >> 29) & 0x7;
  // 12 bits = code
  ins->code = (bin >> 17) & 0xfff;

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
  a0->ext = (bin & 1);
  a1->ext = (bin & 1);

  return true;
}

/** checks the opcode before evaluation */
static bool chck_ins (EVAL_ARGS)
{
  // undefined opcode
  if (ins->code >= NUM_OPS) {
    vm_warn(vm, VM_ESTR_INS);
  #ifndef NDEBUG
    printf("opcode number: 0x%x\n", ins->code);
  #endif
    return false;
  }
  // only one argument can use memory-addressing
  if (ins->args[0].type == ARG_IRM &&
      ins->args[1].type == ARG_IRM) {
    vm_warn(vm, VM_ESTR_IRM);
    return false;
  }
  // all checks passed
  return true;
}

/** evaluates a opcode */
static void eval_ins (EVAL_ARGS)
{
#ifndef NDEBUG
  vm_ops ops = vm->ops[ins->code];
  printf(
    "OPCODE: %s (0x%x)\n", 
    ops ? ops : "[built-in]",
    ins->code
  );
#endif

  switch (ins->code) {
    case OPC_NOP:
      return;
    case OPC_HLT:
      vm_exit(vm, VM_EHLT);
      return;
  }

  vm_oph oph = vm->oph[ins->code];
  
  if (oph == 0) {
    vm_exit(vm, VM_EOPH);
    return;
  }

  (oph)(EVAL_PASS);
}
