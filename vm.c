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
  struct vm_opc *opc

#define EVAL_ARGS     \
  struct vm *vm,      \
  struct vm_opc *opc

#define EVAL_PASS vm, opc

/** reads raw memory */
static bool read_mem (struct vm*, szt, void*);
/** reads a opcode */
static bool read_opc (READ_ARGS);

/** checks the opcode before evaluation */
static bool chck_opc (EVAL_ARGS);

/** debug opcode */
static void dump_opc (struct vm_opc *);
/** debug vm state */
static void dump_rvm(struct vm*);

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
  vm->pc = 0;
  vm->st = 0;
  vm->ip = 0;
  vm->ep = 0;
  vm->err = 0;
  vm->hlt = 0;

  // general purpose registers
  vm->r0 = 1;
  vm->r1 = 0;
  vm->r2 = 0;
  
  // initialize stack
  vm->mm = mem_aodt(512);
  vm->sp = (intptr_t) (vm->mm + 512);
  vm->bp = vm->sp;

  // memory boundaries
  vm->mn = (intptr_t) (vm->mm);
  vm->mx = (intptr_t) (vm->mm + 512);

  // built-in ops (not dispatched)
  vm->oph[OPI_NOP] = 0;
  vm->oph[OPI_HLT] = 0;

  #include "vm.tab"
}

VM_CALL void vm_exit (struct vm *vm, enum vm_err err)
{
  vm->hlt = true;
  vm->err = err;

#ifndef NDEBUG
  printf("ERROR: %u\n", err);
#endif
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_flag (struct vm *vm, u32 flag, bool set)
{
  if (set) {
    vm->st |= flag;
    return;
  }
  vm->st &= ~flag;
}

/**
 * {@inheritdoc}
 */
VM_CALL bool vm_fchk (struct vm *vm, u32 flag)
{
  if (vm->st & flag) {
    return true;
  }
  return false;
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

  struct vm_opc opc;
  
  vm->pc = 0;
  vm->ip = ops;
  vm->ep = ops + len;

  while ((vm->ip + vm->pc) < vm->ep) {
    memset(&opc, 0, sizeof(opc));

    if (!read_opc(vm, &opc)) {
      vm_exit(vm, VM_EOPC);
      goto end;
    }

    if (!chck_opc(vm, &opc)) {
      vm_exit(vm, VM_ECHK);
      goto end;
    }
    
    eval_opc(vm, &opc);

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
VM_CALL bool vm_args (struct vm *vm, const char *fmt, ...)
{
  assert(vm != 0);
  assert(fmt != 0);

  return false;
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
  memcpy(out, vm->ip + (vm->pc), sz);
  vm->pc += sz;
  return true;
}

/** reads a opcode */
static bool read_opc (READ_ARGS)
{
  u32 bin; // opcode
  u16 ext; // opcode extra
  
  if (!read_mem(vm, 4, &bin)) {
    // read error, abort
    return false;
  }

  // 4 bits = mode
  opc->mode = (bin >> 28) & 0xf;
  // 8 bits = code
  opc->code = (bin >> 20) & 0xff;

  struct vm_arg 
    *a0 = opc->args + 0,
    *a1 = opc->args + 1;

  // 3 bits = arg 0 type
  a0->type = (bin >> 17) & 0x7;
  // 4 bits = arg 0 reg
  a0->reg = (bin >> 13) & 0xf;
  // 8 bits = arg 0 off
  a0->off = (bin >> 5) & 0xff;

  // argument 1 type
  a1->type = (bin >> 2) & 0x7;

  // data indicator
  switch (bin & 0x5) {
    case EXT_ARG: break;
    case EXT_END: return true;
    default:
      assert(0);
      return false;
  }

  if (!read_mem(vm, 2, &ext)) {
    // read error, discard and abort
    return false;
  }

  // 4 bits = arg 1 reg
  a1->reg = (ext >> 12) & 0xf;
  // 8 bits = arg 1 off
  a1->off = (ext >> 4) & 0xff;

  return true;
}

/** checks the opcode before evaluation */
static bool chck_opc (EVAL_ARGS)
{
  // all checks passed
  return true;
}

static void dump_opc (struct vm_opc *opc)
{
  printf(
    "mode: %d\n"
    "code: %u\n"
    "args[0] = {\n"
    "  type: %d\n"
    "  reg: %u\n"
    "  off: %u\n"
    "}\n"
    "args[1] = {\n"
    "  type: %d\n"
    "  reg: %u\n"
    "  off: %u\n"
    "}\n",
    opc->mode,
    opc->code,
    opc->args[0].type,
    opc->args[0].reg,
    opc->args[0].off,
    opc->args[1].type,
    opc->args[1].reg,
    opc->args[1].off
  );
}

static void dump_rvm (struct vm *vm)
{
  printf(
    "PC = %u\n"
    "ST = %u\n"
    "IP = %p\n"
    "EP = %p\n"
    "MM = %p\n"
    "SP = %p\n"
    "BP = %p\n"
    "MN = %p\n"
    "MX = %p\n"
    "R0 = %lu\n"
    "R1 = %lu\n"
    "R2 = %lu\n",
    vm->pc,
    vm->st,
    vm->ip,
    vm->ep,
    vm->mm,
    (void*) vm->sp,
    (void*) vm->bp,
    (void*) vm->mn,
    (void*) vm->mx,
    vm->r0,
    vm->r1,
    vm->r2
  );
}

/** evaluates a opcode */
static void eval_opc (EVAL_ARGS)
{
  if (opc->code == OPI_NOP) {
    return;
  }

  if (opc->code == OPI_HLT) {
    vm_exit(vm, VM_EHLT);
    return;
  }

  vm_oph oph = vm->oph[opc->code];
  
  if (oph == 0) {
    vm_exit(vm, VM_EOPH);
    return;
  }

  #ifndef NDEBUG
    vm_ops ops = vm->ops[opc->code];
    printf(
      "OPCOE: %s (0x%x)\n", 
      ops ? ops : "(builtin)",
      opc->code
    );
  #endif

  (oph)(EVAL_PASS);
}
