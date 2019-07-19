#pragma once

/**
 * this file is the run-time support library.
 * here are all memory read/write ops managed.
 */

#include "vm.h"
#include "def.h"

#define DSP_ARGS struct vm *vm
#define IMM_ARGS struct vm *vm
#define RUN_ARGS struct vm *vm, struct vm_arg *arg

#define MEM_ARGS struct vm *vm, intptr_t ptr
#define MEM_PASS vm, ptr

/**
 * Memory API
 */

extern intptr_t mrm_rd (RUN_ARGS);

extern void memw_8 (MEM_ARGS, u8);
extern u8   memr_8 (MEM_ARGS);
extern void memw_16 (MEM_ARGS, u16);
extern u16  memr_16 (MEM_ARGS);
extern void memw_32 (MEM_ARGS, u32);
extern u32  memr_32 (MEM_ARGS);
EX64 extern void memw_64 (MEM_ARGS, u64);
EX64 extern u64  memr_64 (MEM_ARGS);

extern void     memw_a (MEM_ARGS, intptr_t);
extern intptr_t memr_a (MEM_ARGS);

/**
 * ModR/M API
 */

extern void mrmw_8 (RUN_ARGS, u8);
extern u8   mrmr_8 (RUN_ARGS);
extern void mrmw_16 (RUN_ARGS, u16);
extern u16  mrmr_16 (RUN_ARGS);
extern void mrmw_32 (RUN_ARGS, u32);
extern u32  mrmr_32 (RUN_ARGS);
EX64 extern void mrmw_64 (RUN_ARGS, u64);
EX64 extern u64  mrmr_64 (RUN_ARGS);

extern void     mrmw_a (RUN_ARGS, intptr_t);
extern intptr_t mrmr_a (RUN_ARGS);

/**
 * Register API
 */

extern void regw_8 (RUN_ARGS, u8);
extern u8   regr_8 (RUN_ARGS);
extern void regw_16 (RUN_ARGS, u16);
extern u16  regr_16 (RUN_ARGS);
extern void regw_32 (RUN_ARGS, u32);
extern u32  regr_32 (RUN_ARGS);
EX64 extern void regw_64 (RUN_ARGS, u64);
EX64 extern u64  regr_64 (RUN_ARGS);

/**
 * Immediate API
 */

extern u8  immr_8 (IMM_ARGS);
extern u16 immr_16 (IMM_ARGS);
extern u32 immr_32 (IMM_ARGS);
EX64 extern u64 immr_64 (IMM_ARGS);

/**
 * Displacement API
 */

extern intptr_t dspr_a (DSP_ARGS);
