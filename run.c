#include "run.h"

#include <stdio.h>

/* read address or register */
static u8 rptr_8 (struct vm*, intptr_t);
static u8 rreg_8 (struct vm*, struct vm_arg*);
static u8 rptr_16 (struct vm*, intptr_t);
static u8 rreg_16 (struct vm*, struct vm_arg*);
static u8 rptr_32 (struct vm*, intptr_t);
static u8 rreg_32 (struct vm*, struct vm_arg*);
static u8 rptr_64 (struct vm*, intptr_t);
static u8 rreg_64 (struct vm*, struct vm_arg*);

/* write address or register */
static void wptr_8 (struct vm*, intptr_t, u8);
static void wreg_8 (struct vm*, struct vm_arg*, u8);
static void wptr_16 (struct vm*, intptr_t, u16);
static void wreg_16 (struct vm*, struct vm_arg*, u16);
static void wptr_32 (struct vm*, intptr_t, u32);
static void wreg_32 (struct vm*, struct vm_arg*, u32);
static void wptr_64 (struct vm*, intptr_t, u64);
static void wreg_64 (struct vm*, struct vm_arg*, u64);

#define CSIP(N,A) (u ## N)((intptr_t)(A))
#define SZIP sizeof(intptr_t)

#define MEMR(N,S,I) do {                        \
  switch (arg->type) {                          \
    case ARG_ADR:                               \
      if (SZIP != S) {                          \
        vm_warn(vm, "illegal use of address");  \
        vm_flag(vm, FLG_HLT);                   \
        return 0;                               \
      }                                         \
      switch (arg->reg) {                       \
        case REG_BP: return CSIP(N, vm->bp);    \
        case REG_SP: return CSIP(N, vm->sp);    \
        case REG_R0: return CSIP(N, &(vm->r0)); \
        case REG_R1: return CSIP(N, &(vm->r1)); \
        case REG_R2: return CSIP(N, &(vm->r2)); \
        default:                                \
          if (imm->size != S) {                 \
            vm_warn(vm, "invalid argument");    \
            vm_flag(vm, FLG_HLT);               \
            return 0;                           \
          }                                     \
          return imm->data.I;                   \
      }                                         \
    case ARG_PTR: {                             \
      switch (arg->reg) {                       \
        case REG_BP:                            \
        case REG_SP:                            \
          return rreg_ ## N (vm, arg);          \
        case REG_R0:                            \
          return vm->r0 & MSK_ ## N;            \
        case REG_R1:                            \
          return vm->r1 & MSK_ ## N;            \
        case REG_R2:                            \
          return vm->r2 & MSK_ ## N;            \
      }                                         \
      if (imm->size != SZIP) {                  \
        vm_warn(vm, "invalid argument");        \
        vm_flag(vm, FLG_HLT);                   \
        return 0;                               \
      }                                         \
      intptr_t p = IMM_PP(imm);                 \
      p += arg->off;                            \
      return rptr_ ## N(vm, p);                 \
    }                                           \
    default:                                    \
      if (imm->size != S) {                     \
        vm_warn(vm, "invalid argument");        \
        vm_flag(vm, FLG_HLT);                   \
        return 0;                               \
      }                                         \
      return imm->data.I;                       \
  }                                             \
} while (0)

#define MEMW(N,S,M) do {                                  \
  switch (arg->type) {                                    \
    case ARG_ADR:                                         \
      if (SZIP != S) {                                    \
        vm_warn(vm, "illegal use of address");            \
        vm_flag(vm, FLG_HLT);                             \
        return;                                           \
      }                                                   \
      switch (arg->reg) {                                 \
        case REG_BP:                                      \
          vm->bp = (u8*)((intptr_t)(val));                \
          return;                                         \
        case REG_SP:                                      \
          vm->sp = (u8*)((intptr_t)(val));                \
          return;                                         \
        case REG_R0:                                      \
        case REG_R1:                                      \
        case REG_R2:                                      \
          vm_warn(vm, "illegal use of scratch register"); \
          vm_flag(vm, FLG_HLT);                           \
          return;                                         \
        default:                                          \
          vm_warn(vm, "cannot set address of immediate"); \
          vm_flag(vm, FLG_HLT);                           \
          return;                                         \
      }                                                   \
    case ARG_PTR: {                                       \
      switch (arg->reg) {                                 \
        case REG_BP:                                      \
        case REG_SP:                                      \
          wreg_ ## N (vm, arg, val);                      \
          return;                                         \
        case REG_R0:                                      \
          vm->r0 &= M;                                    \
          vm->r0 |= val;                                  \
          return;                                         \
        case REG_R1:                                      \
          vm->r1 &= M;                                    \
          vm->r1 |= val;                                  \
          return;                                         \
        case REG_R2:                                      \
          vm->r2 &= M;                                    \
          vm->r2 |= val;                                  \
          return;                                         \
      }                                                   \
      if (imm->size != SZIP) {                            \
        vm_warn(vm, "invalid argument");                  \
        vm_flag(vm, FLG_HLT);                             \
        return;                                           \
      }                                                   \
      intptr_t p = IMM_PP(imm);                           \
      p += arg->off;                                      \
      wptr_ ## N (vm, p, val);                            \
      return;                                             \
    }                                                     \
    default:                                              \
      vm_warn(vm, "cannot write to immediate");           \
      vm_flag(vm, FLG_HLT);                               \
  }                                                       \
} while (0)

/**
 * {@inheritdoc}
 */
RUN_CALL u8 memr_8 (RUN_ARGS)
{
  MEMR(8, 1, byte);
}

/**
 * {@inheritdoc}
 */
RUN_CALL u16 memr_16 (RUN_ARGS)
{
  MEMR(16, 2, word);
}

/**
 * {@inheritdoc}
 */
RUN_CALL u32 memr_32 (RUN_ARGS)
{
  MEMR(32, 4, dword);
}

/**
 * {@inheritdoc}
 */
RUN_CALL u64 memr_64 (RUN_ARGS)
{
  MEMR(64, 8, qword);
}

/**
 * {@inheritdoc}
 */
RUN_CALL void memw_8 (RUN_ARGS, u8 val)
{
  MEMW(8, 1, MSK_8);
}

/**
 * {@inheritdoc}
 */
RUN_CALL void memw_16 (RUN_ARGS, u16 val)
{
  MEMW(16, 2, MSK_16);
}

/**
 * {@inheritdoc}
 */
RUN_CALL void memw_32 (RUN_ARGS, u32 val)
{
  MEMW(32, 4, MSK_32);
}

/**
 * {@inheritdoc}
 */
RUN_CALL void memw_64 (RUN_ARGS, u64 val)
{
  MEMW(64, 8, MSK_64);
}

#define PTRR(N,S) do {                          \
  if (ptr == 0) {                               \
    vm_warn(vm, "null address read");           \
    vm_flag(vm, FLG_HLT);                       \
    return 0;                                   \
  }                                             \
  if (ptr < vm->mn || (ptr + (S-1)) > vm->mx) { \
    vm_warn(vm, "address out of bounds");       \
    vm_flag(vm, FLG_HLT);                       \
    return 0;                                   \
  }                                             \
  return *(u ## N*)ptr;                         \
} while (0)

#define PTRW(N,S) do {                          \
  if (ptr == 0) {                               \
    vm_warn(vm, "null address write");          \
    vm_flag(vm, FLG_HLT);                       \
    return;                                     \
  }                                             \
  if (ptr < vm->mn || (ptr + (S-1)) > vm->mx) { \
    vm_warn(vm, "address out of bounds");       \
    vm_flag(vm, FLG_HLT);                       \
    return;                                     \
  }                                             \
  *((u ## N*)ptr) = val;                        \
} while (0)

#define REGR(N) do {                        \
  intptr_t p = 0;                           \
  switch (arg->reg) {                       \
    case REG_BP:                            \
      p = (intptr_t) vm->bp;                \
      break;                                \
    case REG_SP:                            \
      p = (intptr_t) vm->sp;                \
      break;                                \
    default:                                \
      vm_warn(vm, "not a memory register"); \
      vm_flag(vm, FLG_HLT);                 \
      return 0;                             \
  }                                         \
  p += arg->off;                            \
  return rptr_ ## N (vm, p);                \
} while (0)

#define REGW(N) do {                        \
  intptr_t p = 0;                           \
  switch (arg->reg) {                       \
    case REG_BP:                            \
      p = (intptr_t) vm->bp;                \
      break;                                \
    case REG_SP:                            \
      p = (intptr_t) vm->sp;                \
      break;                                \
    default:                                \
      vm_warn(vm, "not a memory register"); \
      vm_flag(vm, FLG_HLT);                 \
      return;                               \
  }                                         \
  wptr_ ## N (vm, p, val);                  \
} while (0)

static u8 rptr_8 (struct vm *vm, intptr_t ptr)
{
  PTRR(8, 1);
}

static u8 rreg_8 (struct vm *vm, struct vm_arg *arg)
{
  REGR(8);
}

static u8 rptr_16 (struct vm *vm, intptr_t ptr)
{
  PTRR(16, 2);
}

static u8 rreg_16 (struct vm *vm, struct vm_arg *arg)
{
  REGR(16);
}

static u8 rptr_32 (struct vm *vm, intptr_t ptr)
{
  PTRR(32, 4);
}

static u8 rreg_32 (struct vm *vm, struct vm_arg *arg)
{
  REGR(32);
}

static u8 rptr_64 (struct vm *vm, intptr_t ptr)
{
  PTRR(64, 8);
}

static u8 rreg_64 (struct vm *vm, struct vm_arg *arg)
{
  REGR(64);
}

static void wptr_8 (struct vm *vm, intptr_t ptr, u8 val)
{
  PTRW(8, 1);
}

static void wreg_8 (struct vm *vm, struct vm_arg *arg, u8 val)
{
  REGW(8);
}

static void wptr_16 (struct vm *vm, intptr_t ptr, u16 val)
{
  PTRW(16, 2);
}

static void wreg_16 (struct vm *vm, struct vm_arg *arg, u16 val)
{
  REGW(16);
}

static void wptr_32 (struct vm *vm, intptr_t ptr, u32 val)
{
  PTRW(32, 4);
}

static void wreg_32 (struct vm *vm, struct vm_arg *arg, u32 val)
{
  REGW(32);
}

static void wptr_64 (struct vm *vm, intptr_t ptr, u64 val)
{
  PTRW(64, 8);
}

static void wreg_64 (struct vm *vm, struct vm_arg *arg, u64 val)
{
  REGW(64);
}
