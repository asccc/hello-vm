#pragma once

#include "vm.h"
#include "def.h"

#define FL_CALL __attribute__((nonnull)) void

#if VM_HAS_QWORD
  #define FL_EX64 FL_CALL
#else
  #define FL_EX64 FL_CALL __attribute__((unused))
#endif

/**
 * calculates ZF
 * 
 * @param the virtual machine struct
 * @param the flag setting
 */
extern FL_CALL calc_zf (struct vm*, bool);

/**
 * calculates CF
 * 
 * @param the virtual machine struct
 * @param the flag setting
 */
extern FL_CALL calc_cf (struct vm*, bool);

/**
 * calculates SF
 * 
 * @param the virtual machine struct
 * @param the number to test
 */
extern FL_CALL calc_sf_8 (struct vm*, u8);
extern FL_CALL calc_sf_16 (struct vm*, u16);
extern FL_CALL calc_sf_32 (struct vm*, u32);
extern FL_EX64 calc_sf_64 (struct vm*, u64);

/**
 * calculates OF
 * 
 * @param the virtual machine struct
 * @param the result of the operation
 * @param the first operand
 * @param the second operand
 */
extern FL_CALL calc_of_8 (struct vm*, u8, u8, u8);
extern FL_CALL calc_of_16 (struct vm*, u16, u16, u16);
extern FL_CALL calc_of_32 (struct vm*, u32, u32, u32);
extern FL_EX64 calc_of_64 (struct vm*, u64, u64, u64);
