#include "int.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * {@inheritdoc}
 */
INT_CALL void int_exec (struct vm *vm, u32 num)
{
  switch (num) {
    case IC_DUMP: INT_NAME(dump)(vm); break;
    case IC_EXIT: INT_NAME(exit)(vm); break;
    case IC_PUTC: INT_NAME(putc)(vm); break;
    case IC_PUTS: INT_NAME(puts)(vm); break;
    default:
      vm_exit(vm, VM_EUNKI);
  }
}

INT_FUNC(dump)
{ 
  printf(
    "VM STATE {\n"
    "  PC = %u\n"
  	"  ST = %u\n"
    "  IP = %p\n"
    "  EP = %p\n"
    "  MM = %p\n"
    "  SP = %p\n"
    "  BP = %p\n"
    "  MN = %p\n"
    "  MX = %p\n"
    "  R0 = %p\n"
    "  R1 = %p\n"
    "  R2 = %p\n"
    "  HLT = %u\n"
    "  ERR = %u\n"
    "}\n",
    vm->pc,
    vm->st,
    vm->ip,
    vm->ep,
    vm->mm,
    (void*) vm->sp,
    (void*) vm->bp,
    (void*) vm->mn,
    (void*) vm->mx,
    (void*) vm->r0,
    (void*) vm->r1,
    (void*) vm->r2,
    vm->hlt,
    vm->err
  );

  printf(
    "VM FLAGS {"
    "  ZF = %u\n"
    "  CF = %u\n"
    "  SF = %u\n"
    "  OF = %u\n"
    "}\n",
    vm->st & FLG_ZF ? 1 : 0,
    vm->st & FLG_CF ? 1 : 0,
    vm->st & FLG_SF ? 1 : 0,
    vm->st & FLG_OF ? 1 : 0
  );
}

INT_FUNC(putc)
{
  u32 c = vm->r1;
  putc(c, stdout);
}

INT_FUNC(puts)
{
  puts((char*)(vm->r1));
}

INT_FUNC(exit)
{
  vm_exit(vm, vm->r1);
}
