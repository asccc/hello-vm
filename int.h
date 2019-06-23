#pragma once

#include "vm.h"
#include "def.h"

#define INT_CALL __attribute__((nonnull))
#define INT_ATTR __attribute__((nonnull, noinline))
#define INT_NAME(N) vm__int__ ## N
#define INT_FUNC(N) INT_ATTR void INT_NAME(N) (struct vm *vm)

typedef void(*int_fn)(struct vm*);

enum int_op {
  IC_DUMP, // dump vm state
  IC_PUTC, // print a character in R1
  IC_PUTS, // print a zero terminated string via R1
  IC_EXIT, // exit vm with exit status in R1
};

extern INT_FUNC(dump);
extern INT_FUNC(putc);
extern INT_FUNC(puts);
extern INT_FUNC(exit);

/**
 * executes a interrupt call
 * 
 * @param the virtual machine struct
 * @param the interrupt number
 */
extern INT_CALL void int_exec (struct vm*, u32);
