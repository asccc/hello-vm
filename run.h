#pragma once

#include "vm.h"
#include "def.h"

#define RUN_CALL __attribute__((nonnull))

#define RUN_ARGS      \
  struct vm *vm,      \
  struct vm_arg *arg, \
  struct vm_imm *imm

/**
 * reads 8 bits from the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @return the value
 */
extern RUN_CALL u8 memr_8 (RUN_ARGS);

/**
 * reads 16 bits from the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @return the value
 */
extern RUN_CALL u16 memr_16 (RUN_ARGS);

/**
 * reads 32 bits from the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @return the value
 */
extern RUN_CALL u32 memr_32 (RUN_ARGS);

/**
 * reads 64 bits from the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @return the value
 */
extern RUN_CALL u64 memr_64 (RUN_ARGS);

/**
 * writes 8 bits to the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @param the value to write
 */
extern RUN_CALL void memw_8 (RUN_ARGS, u8);

/**
 * writes 16 bits to the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @param the value to write
 */
extern RUN_CALL void memw_16 (RUN_ARGS, u16);

/**
 * writes 32 bits to the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @param the value to write
 */
extern RUN_CALL void memw_32 (RUN_ARGS, u32);

/**
 * writes 64 bits to the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @param the value to write
 */
extern RUN_CALL void memw_64 (RUN_ARGS, u64);
