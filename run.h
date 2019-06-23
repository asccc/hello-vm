#pragma once

/**
 * this file is the run-time support library.
 * here are all memory read/write ops managed.
 */

#include "vm.h"
#include "def.h"

#define RUN_CALL __attribute__((nonnull))

#if VM_HAS_QWORD
  #define RUN_EX64 RUN_CALL
#else
  #define RUN_EX64 RUN_CALL __attribute__((unused))
#endif

#define RUN_ARGS     \
  struct vm *vm,     \
  struct vm_arg *arg

#define MEM_ARGS \
  struct vm *vm, \
  intptr_t ptr

/**
 * Memory API
 */

extern RUN_CALL void memw_8 (MEM_ARGS, u8);
extern RUN_CALL u8   memr_8 (MEM_ARGS);
extern RUN_CALL void memw_16 (MEM_ARGS, u16);
extern RUN_CALL u16  memr_16 (MEM_ARGS);
extern RUN_CALL void memw_32 (MEM_ARGS, u32);
extern RUN_CALL u32  memr_32 (MEM_ARGS);
extern RUN_EX64 void memw_64 (MEM_ARGS, u64);
extern RUN_EX64 u64  memr_64 (MEM_ARGS);

/**
 * ModR/M API
 */

extern RUN_CALL void mrmw_8 (RUN_ARGS, u8);
extern RUN_CALL u8   mrmr_8 (RUN_ARGS);
extern RUN_CALL void mrmw_16 (RUN_ARGS, u16);
extern RUN_CALL u16  mrmr_16 (RUN_ARGS);
extern RUN_CALL void mrmw_32 (RUN_ARGS, u32);
extern RUN_CALL u32  mrmr_32 (RUN_ARGS);
extern RUN_EX64 void mrmw_64 (RUN_ARGS, u64);
extern RUN_EX64 u64  mrmr_64 (RUN_ARGS);

/**
 * Register API
 */

extern RUN_CALL void regw_8 (RUN_ARGS, u8);
extern RUN_CALL u8   regr_8 (RUN_ARGS);
extern RUN_CALL void regw_16 (RUN_ARGS, u16);
extern RUN_CALL u16  regr_16 (RUN_ARGS);
extern RUN_CALL void regw_32 (RUN_ARGS, u32);
extern RUN_CALL u32  regr_32 (RUN_ARGS);
extern RUN_EX64 void regw_64 (RUN_ARGS, u64);
extern RUN_EX64 u64  regr_64 (RUN_ARGS);

/**
 * Immediate API
 */

extern RUN_CALL u8  immr_8 (struct vm*);
extern RUN_CALL u16 immr_16 (struct vm*);
extern RUN_CALL u32 immr_32 (struct vm*);
extern RUN_EX64 u64 immr_64 (struct vm*);
