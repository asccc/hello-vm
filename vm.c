#include "vm.h"
#include "mem.h"
#include "run.h"

#include "op.h"

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#define READ_ARGS         \
  struct vm *vm, u8 *ops, \
  struct vm_opc *opc,     \
  struct vm_imm *imm

#define EVAL_ARGS     \
  struct vm *vm,      \
  struct vm_opc *opc, \
  struct vm_imm *imm

/** reads raw memory */
static bool read_mem (struct vm*, u8*, szt, void*);

/** reads a opcode */
static bool read_opc (READ_ARGS);
/** reads a immediate */
static bool read_imm (READ_ARGS);

/** debug opcode */
static void dump_opc (struct vm_opc *);
/** debug immediate */
static void dump_imm (struct vm_imm *);
/** debug vm state */
static void dump_rvm(struct vm*);

/**
 * {@inheritdoc}
 */
VM_CALL void vm_init (struct vm *vm)
{
  assert(vm != 0);
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_flag (struct vm *vm, u32 flag)
{
  
}

/**
 * {@inheritdoc}
 */
VM_CALL bool vm_fchk (struct vm *vm, u32 flag)
{
  return false;
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_warn (struct vm *vm, const char *msg)
{
  fputs(msg, stderr);
  fputs("\n", stderr);
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_free (struct vm *vm)
{
  assert(vm != 0);

}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_exec (struct vm *vm, u8 *ops)
{
  assert(vm != 0);
  assert(ops != 0);

  struct vm_opc opc;
  struct vm_imm imm;
  enum op_res res = RES_ERR;
  
  vm->pc = 0;
  for (;;) {
    memset(&opc, 0, sizeof(opc));
    memset(&imm, 0, sizeof(imm));

    if (!read_opc(vm, ops, &opc, &imm)) {
      vm_warn(vm, "bytecode error");
      goto end;
    }

    dump_opc(&opc);
    dump_imm(&imm);
    dump_rvm(vm);
    break;

    switch (res) {
      case RES_NXT:
        vm->pc++;
        /* fall through */
      case RES_CNT:
        break;
      case RES_ERR:
        vm_warn(vm, "eval error");
        /* fall through */
      case RES_HLT:
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
static bool read_mem (struct vm *vm, u8 *mem, szt sz, void *out)
{
  memcpy(out, mem + (vm->pc), sz);
  vm->pc += sz;
  return true;
}

/** reads a opcode */
static bool read_opc (READ_ARGS)
{
  u32 bin; // opcode
  u16 ext; // opcode extra
  
  if (!read_mem(vm, ops, 4, &bin)) {
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
      return read_imm(vm, ops, opc, imm);
    default:
      assert(0);
      return false;
  }

  if (!read_mem(vm, ops, 2, &ext)) {
    // read error, discard and abort
    return false;
  }

  // 4 bits = arg 1 reg
  a1->reg = (ext >> 12) & 0xf;
  // 8 bits = arg 1 off
  a1->off = (ext >> 8) & 0xff;

  return true;
}

#define READ_IMM(N,T) vm, ops, N, &(imm->data.T)
#define READ_IMM_8  READ_IMM(1, byte)
#define READ_IMM_16 READ_IMM(2, word)
#define READ_IMM_32 READ_IMM(4, dword)
#define READ_IMM_64 READ_IMM(8, qword)

/** reads a immediate */
static bool read_imm (READ_ARGS)
{
  switch (opc->mode) {
    case MOD_BYTE:
      return read_mem(READ_IMM_8);
    case MOD_WORD:
      return read_mem(READ_IMM_16);
    case MOD_DWORD:
      return read_mem(READ_IMM_32);
  #if VM_USE_QWORD
    case MOD_QWORD:
      return read_mem(READ_IMM_64);
  #endif
    default:
      assert(0);
      return false;
  }
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
    "  byte: %u\n"
    "  word: %u\n"
    "  dword: %u\n"
  #if VM_USE_QWORD
    "  qword: %lu\n"
  #endif
    "}\n",
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
    "PC = %u\n",
    vm->pc
  );
}
