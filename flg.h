#pragma once

#include "vm.h"
#include "def.h"

/**
 * calculates ZF
 * 
 * @param the virtual machine struct
 * @param the flag setting
 */
#define calc_zf(vm, s) (vm)->flg.zf = !(s)

/**
 * calculates CF
 * 
 * @param the virtual machine struct
 * @param the flag setting
 */
#define calc_cf(vm, s) (vm)->flg.cf = (s)

/**
 * calculates SF
 * 
 * @param the virtual machine struct
 * @param the number to test
 */
extern void calc_sf_8 (struct vm*, u8);
extern void calc_sf_16 (struct vm*, u16);
extern void calc_sf_32 (struct vm*, u32);
EX64 extern void calc_sf_64 (struct vm*, u64);

/**
 * calculates OF
 * 
 * @param the virtual machine struct
 * @param the result of the operation
 * @param the first operand
 * @param the second operand
 */
extern void calc_of_8 (struct vm*, u8, u8, u8);
extern void calc_of_16 (struct vm*, u16, u16, u16);
extern void calc_of_32 (struct vm*, u32, u32, u32);
EX64 extern void calc_of_64 (struct vm*, u64, u64, u64);
