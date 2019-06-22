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
  struct vm_opc *opc, \
  struct vm_imm *imm

#define EVAL_ARGS     \
  struct vm *vm,      \
  struct vm_opc *opc, \
  struct vm_imm *imm

#define EVAL_PASS vm, opc, imm

/** reads raw memory */
static bool read_mem (struct vm*, szt, void*);

/** reads a opcode */
static bool read_opc (READ_ARGS);
/** reads a immediate */
static bool read_imm (READ_ARGS);

/** checks the opcode before evaluation */
static bool chck_opc (EVAL_ARGS);

/** debug opcode */
static void dump_opc (struct vm_opc *);
/** debug immediate */
static void dump_imm (struct vm_imm *);
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

  // initialize context
  vm->pc = 0;
  vm->st = 0;
  vm->ip = 0;
  vm->ep = 0;

  // general purpose registers
  vm->r0 = mem_aodt(sizeof(vm_max));
  vm->r1 = mem_aodt(sizeof(vm_max));
  vm->r2 = mem_aodt(sizeof(vm_max));
  
  // initialize stack
  vm->mm = mem_aodt(512);
  vm->sp = vm->mm + 512;
  vm->bp = vm->sp;

  // memory boundaries
  vm->mn = (intptr_t) (vm->mm);
  vm->mx = (intptr_t) (vm->mm + 512);

  // built-in ops (not dispatched)
  vm->oph[OP_NOP] = 0;
  vm->oph[OP_HLT] = 0;

  #include "vm.tab"
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
  fputs("[WARN]: ", stderr);
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
  mem_free(vm->r0);
  mem_free(vm->r1);
  mem_free(vm->r2);
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_exec (struct vm *vm, u8 *ops, szt len)
{
  assert(vm != 0);
  assert(ops != 0);

  struct vm_opc opc;
  struct vm_imm imm;
  
  vm->pc = 0;
  vm->ip = ops;
  vm->ep = ops + len;

  while ((vm->ip + vm->pc) < vm->ep) {
    memset(&opc, 0, sizeof(opc));
    memset(&imm, 0, sizeof(imm));

    if (!read_opc(vm, &opc, &imm)) {
      vm_warn(vm, "bytecode error");
      goto end;
    }

  #ifndef NDEBUG
    dump_opc(&opc);
    dump_imm(&imm);
  #endif

    if (!chck_opc(vm, &opc, &imm)) {
      vm_warn(vm, "check failed");
      goto end;
    }
    
    eval_opc(vm, &opc, &imm);

  #ifndef NDEBUG
    dump_rvm(vm);
  #endif

    if (vm_fchk(vm, FLG_HLT)) {
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

  // clear immediate
  imm->size = 0;
  
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

  // 2 bits = arg 0 type
  a0->type = (bin >> 18) & 0x3;
  // 4 bits = arg 0 reg
  a0->reg = (bin >> 14) & 0xf;
  // 8 bits = arg 0 off
  a0->off = (bin >> 6) & 0xff;

  // argument 1 type
  a1->type = (bin >> 4) & 0x3;

  // data indicator
  switch (bin & 0xf) {
    case EXT_ARG: break;
    case EXT_END: return true;
    case EXT_IMM:
      return read_imm(vm, opc, imm);
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

#define READ_IMM(N,T) do {                \
  imm->size = N;                          \
  return read_mem(vm, N, &(imm->data.T)); \
} while (0)

#define READ_IMM_8  READ_IMM(1, byte)
#define READ_IMM_16 READ_IMM(2, word)
#define READ_IMM_32 READ_IMM(4, dword)
#define READ_IMM_64 READ_IMM(8, qword)

/** reads a immediate */
static bool read_imm (READ_ARGS)
{
  switch (opc->mode) {
    case MOD_BYTE:  READ_IMM_8;
    case MOD_WORD:  READ_IMM_16;
    case MOD_DWORD: READ_IMM_32;
  #if VM_USE_QWORD
    case MOD_QWORD: READ_IMM_64;
  #endif
    default: {
      assert(0);
      return false;
    }
  }
}

/** checks the opcode before evaluation */
static bool chck_opc (EVAL_ARGS)
{
  // number of bytes for immediate/address
  u32 size = 1 << (opc->mode - 1);

  // 1. only one immediate is allowed
  if ((opc->args[0].type == ARG_IMM ||
       (opc->args[0].type == ARG_PTR &&
        opc->args[0].reg == 0)) &&
      (opc->args[1].type == ARG_IMM ||
       (opc->args[1].type == ARG_PTR &&
        opc->args[1].reg == 0))) {
    vm_warn(vm, "a instruction can only "
                "request one immediate");
    return false;
  }

  // 2. a immediate is required
  if (((opc->args[0].type == ARG_PTR &&
        opc->args[0].reg == 0) ||
       opc->args[0].type == ARG_IMM ||
       (opc->args[1].type == ARG_PTR &&
        opc->args[1].reg == 0) ||
       opc->args[1].type == ARG_IMM) &&
      imm->size == 0) {
    vm_warn(vm, "an immediate is required, "
                "but nothing was loaded");
    return false;
  }

  // 3. immediate size must match
  if (imm->size > 0) {
    u32 reqs = size;
    if (opc->args[0].type == ARG_PTR ||
        opc->args[1].type == ARG_PTR) {
      reqs = sizeof(intptr_t);
    }

    if (imm->size != reqs) {
      vm_warn(vm, "unexpected size of immediate");
    #ifndef NDEBUG
      printf("%u <> %u\n", imm->size, size);
    #endif
      return false;
    }
  }

  // 4. if a pointer is requested
  if ((opc->args[0].type == ARG_PTR ||
       opc->args[1].type == ARG_PTR) &&
      sizeof(intptr_t) != size) {
    vm_warn(vm, "address size mismatch");
    return false;
  }

  // 5. R0, R1, R2 do not support offsets
  // sub r0, [r1 + 4]
  //          ^^^^^^
  if (((opc->args[0].reg == REG_R0 ||
        opc->args[0].reg == REG_R1 ||
        opc->args[0].reg == REG_R1) &&
       opc->args[0].type == ARG_PTR &&
       opc->args[0].off != 0) ||
      ((opc->args[1].reg == REG_R0 ||
        opc->args[1].reg == REG_R1 ||
        opc->args[1].reg == REG_R2) &&
       opc->args[1].type == ARG_PTR &&
       opc->args[1].off != 0)) {
    vm_warn(vm, "scratch register with offset");
    return false;
  }

  // 6. R0, R1, R2 cannot be used as address
  

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

static void dump_imm (struct vm_imm *imm)
{
  printf(
    "imm {\n"
    "  size: %u\n"
    "  data {\n"
    "    byte: %u\n"
    "    word: %u\n"
    "    dword: %u\n"
  #if VM_USE_QWORD
    "    qword: %lu\n"
  #endif
    "  }\n"
    "}\n",
    imm->size,
    imm->data.byte,
    imm->data.word,
    imm->data.dword
  #if VM_USE_QWORD
    ,
    imm->data.qword
  #endif
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
    vm->sp,
    vm->bp,
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
  if (opc->code == OP_NOP) {
    return;
  }

  if (opc->code == OP_HLT) {
    vm_flag(vm, FLG_HLT);
    return;
  }

  vm_oph oph = vm->oph[opc->code];
  
  if (oph == 0) {
    vm_warn(vm, "unknown opcode");
    vm_flag(vm, FLG_HLT);
    return;
  }

  #ifndef NDEBUG
    printf(
      "OP %x -> %s\n", 
      opc->code, 
      vm->ops[opc->code]
    );
  #endif

  (oph)(EVAL_PASS);
}
