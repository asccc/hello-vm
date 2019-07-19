#include "run.h"

/**
 * Displacement API
 */

intptr_t dspr_a (DSP_ARGS) 
{
  switch (vm->opm) {
    case MOD_DWORD: {
      i32 dsp = 0;
      vm_read(vm, sizeof(dsp), &dsp);
      return (intptr_t) dsp;
    }
    case MOD_QWORD: {
      i64 dsp = 0;
      vm_read(vm, sizeof(dsp), &dsp);
      return (intptr_t) dsp;
    }
    default:
      vm_exit(vm, VM_EMOD);
      return 0;
  }
}

/**
 * Memory API
 */

#define MEMW(N)                          \
  void memw_ ## N (MEM_ARGS, u ## N val) \
  {                                      \
    if (ptr < vm->mn) {                  \
      vm_exit(vm, VM_EMBND);             \
      return;                            \
    }                                    \
    if (ptr + (N / 8) > vm->mx) {        \
      vm_exit(vm, VM_EMBND);             \
      return;                            \
    }                                    \
    *((u ## N*) ptr) = val;              \
  }

#define MEMR(N)                          \
  u ## N memr_ ## N (MEM_ARGS)           \
  {                                      \
    if (ptr < vm->mn) {                  \
      vm_exit(vm, VM_EMBND);             \
      return 0;                          \
    }                                    \
    if (ptr + (N / 8) > vm->mx) {        \
      vm_exit(vm, VM_EMBND);             \
      return 0;                          \
    }                                    \
    return *(u ## N*) ptr;               \
  }

MEMR(8)
MEMW(8)
MEMR(16)
MEMW(16)
MEMR(32)
MEMW(32)

#if VM64
  MEMR(64) 
  MEMW(64)
#endif

void memw_a (MEM_ARGS, intptr_t val)
{
#if VM64
  memw_64(MEM_PASS, val);
#else
  memw_32(MEM_PASS, val);
#endif
}

intptr_t memr_a (MEM_ARGS)
{
#if VM64
  return (intptr_t) memr_64(MEM_PASS);
#else
  return (intptr_t) memr_32(MEM_PASS);
#endif
}

/**
 * ModR/M API
 */

intptr_t mrm_rd (RUN_ARGS) 
{
  intptr_t dsp = 
    arg->ext ? dspr_a(vm) : 0;
  switch (arg->reg) {
    case REG_R0: return vm->r0 + dsp;
    case REG_R1: return vm->r1 + dsp;
    case REG_R2: return vm->r2 + dsp;
    case REG_BP: return vm->bp + dsp;
    case REG_SP: return vm->sp + dsp;
    case REG_DS: return vm->ds + dsp;
    case REG_CS: return vm->cs + dsp;
    default:
      if (arg->ext) {
        // if no register was specified
        // but a displacement is set
        return dsp;
      }
      // no register and no displacement
      vm_exit(vm, VM_EUNKR);
      return 0;
  }
}

#define MRMW(N)                                  \
  void mrmw_ ## N (RUN_ARGS, u ## N val)         \
  {                                              \
    switch (arg->type) {                         \
      case ARG_IRM:                              \
        memw_ ## N (vm, mrm_rd(vm, arg), val);   \
        break;                                   \
      case ARG_REG:                              \
        regw_ ## N (vm, arg, val);               \
        break;                                   \
      default:                                   \
        vm_exit(vm, VM_EINMR);                   \
    }                                            \
  }

#define MRMR(N) \
  u ## N mrmr_ ## N (RUN_ARGS)                   \
  {                                              \
    switch (arg->type) {                         \
      case ARG_IRM:                              \
        return memr_ ## N (vm, mrm_rd(vm, arg)); \
      case ARG_REG:                              \
        return regr_ ## N (vm, arg);             \
      default:                                   \
        vm_exit(vm, VM_EINMR);                   \
        return 0;                                \
    }                                            \
  }

MRMR(8)
MRMW(8)
MRMR(16)
MRMW(16)
MRMR(32)
MRMW(32)

#if VM64
  MRMR(64) 
  MRMW(64)
#endif

/**
 * Register API
 */

#define REGR(N)                          \
  u ## N regr_ ## N (RUN_ARGS)           \
  {                                      \
    switch (arg->reg) {                  \
      case REG_R0:                       \
        return vm->r0;                   \
      case REG_R1:                       \
        return vm->r1;                   \
      case REG_R2:                       \
        return vm->r2;                   \
      case REG_BP:                       \
        return vm->bp;                   \
      case REG_SP:                       \
        return vm->sp;                   \
    }                                    \
    vm_exit(vm, VM_EUNKR);               \
    return 0;                            \
  }

#define REGW(N,M)                        \
  void regw_ ## N (RUN_ARGS, u ## N val) \
  {                                      \
    switch (arg->reg) {                  \
      case REG_R0:                       \
        vm->r0 &= M;                     \
        vm->r0 |= val;                   \
        return;                          \
      case REG_R1:                       \
        vm->r1 &= M;                     \
        vm->r1 |= val;                   \
        return;                          \
      case REG_R2:                       \
        vm->r2 &= M;                     \
        vm->r2 |= val;                   \
        return;                          \
      case REG_BP:                       \
        vm->bp &= M;                     \
        vm->bp |= val;                   \
        return;                          \
      case REG_SP:                       \
        vm->sp &= M;                     \
        vm->sp |= val;                   \
        return;                          \
    }                                    \
    vm_exit(vm, VM_EUNKR);               \
  }

REGR(8)
REGW(8, MSK_8)
REGR(16)
REGW(16, MSK_16)
REGR(32)
REGW(32, MSK_32)

#if VM64
  REGR(64) 
  REGW(64, MSK_64)
#endif

/**
 * Immediate API
 */

#define IMMR(N)                  \
  u ## N immr_ ## N (IMM_ARGS)   \
  {                              \
    u ## N out = 0;              \
    vm_read(vm, (N / 8u), &out); \
    return out;                  \
  }

IMMR(8)
IMMR(16)
IMMR(32)

#if VM64
  IMMR(64)
#endif
