#include "op.h"
#include "run.h"

#define OP_IMPL(F,N,T,S)                           \
OP_CALL F ## _m ## N (OP_ARGS)                     \
{                                                  \
  u ## N _0 = memr_ ## N (vm, opc->args + 0, imm); \
  u ## N _1 = memr_ ## N (vm, opc->args + 1, imm); \
  u ## N _r = F ## _ ## S ## N (vm, _0, _1);       \
  memw_ ## N (vm, opc->args + 0, imm, _r);         \
}

static u8 op_add_u8 (struct vm*, u8, u8);
static u8 op_sub_u8 (struct vm*, u8, u8);
static u16 op_add_u16 (struct vm*, u16, u16);
static u16 op_sub_u16 (struct vm*, u16, u16);
static u32 op_add_u32 (struct vm*, u32, u32);
static u32 op_sub_u32 (struct vm*, u32, u32);
static u64 op_add_u64 (struct vm*, u64, u64);
static u64 op_sub_u64 (struct vm*, u64, u64);

OP_IMPL(op_add, 8, byte, u)
OP_IMPL(op_sub, 8, byte, u)

OP_IMPL(op_add, 16, word, u)
OP_IMPL(op_sub, 16, word, u)

OP_IMPL(op_add, 32, dword, u)
OP_IMPL(op_sub, 32, dword, u)

#if VM_HAS_QWORD
  OP_IMPL(op_add, 64, qword, u)
  OP_IMPL(op_sub, 64, qword, u)
#endif

#define BINOP(N,O) do { \
  u ## N _r = a O b;    \
  if (_r == 0) {        \
    vm_flag(vm, FLG_Z); \
  }                     \
  return _r;            \
} while (0)

static u8 op_add_u8 (struct vm *vm, u8 a, u8 b)
{
  BINOP(8, +);
}

static u8 op_sub_u8 (struct vm *vm, u8 a, u8 b)
{
  BINOP(8, -);
}

static u16 op_add_u16 (struct vm *vm, u16 a, u16 b)
{
  BINOP(16, +);
}

static u16 op_sub_u16 (struct vm *vm, u16 a, u16 b)
{
  BINOP(16, -);
}

static u32 op_add_u32 (struct vm *vm, u32 a, u32 b)
{
  BINOP(32, +);
}

static u32 op_sub_u32 (struct vm *vm, u32 a, u32 b)
{
  BINOP(32, -);
}

#if VM_HAS_QWORD
  static u64 op_add_u64 (struct vm *vm, u64 a, u64 b)
  {
    BINOP(64, +);
  }

  static u64 op_sub_u64 (struct vm *vm, u64 a, u64 b)
  {
    BINOP(64, -);
  }
#endif
