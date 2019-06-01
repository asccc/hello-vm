#include "vm.h"

#include <stdio.h>

#define OP0(OP) { .kind = OPC_ ## OP, .args = {0} }
#define OP1(OP, A1) { .kind = OPC_ ## OP, .args[0] = A1 }
#define OP2(OP, A1, A2) { .kind = OPC_ ## OP, .args = {A1, A2} }

#define REG(ID) { .type = OPT_REG, .data.reg = REG_ ## ID }
#define VAL(VL) { .type = OPT_VAL, .data.val = VL }
#define FNC(FN) { .type = OPT_FNC, .data.fnc = VM_NAME(FN) }

VM_FUNC(echo_test) {
  putc((char) (vm->r0 >> 56) & 0xff, stdout);
  putc((char) (vm->r0 >> 48) & 0xff, stdout);
  putc((char) (vm->r0 >> 40) & 0xff, stdout);
  putc((char) (vm->r0 >> 32) & 0xff, stdout);
  putc((char) (vm->r0 >> 24) & 0xff, stdout);
  putc((char) (vm->r0 >> 16) & 0xff, stdout);
  putc((char) (vm->r0 >> 8) & 0xff, stdout);
  putc((char) vm->r0, stdout);
}

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  struct vm_op ops[] = {
    OP2(MOV,  REG(R0), VAL(0x6f726c6421000000)),
    OP1(PUSH, REG(R0)),
    OP2(MOV,  REG(R0), VAL(0x48656c6c6f2c2077)),
    OP1(PUSH, REG(R0)),

    OP2(MOV,  REG(R1), VAL(8)),
    OP1(POP,  REG(R0)),
    OP1(VRT,  FNC(echo_test)),
    OP1(POP,  REG(R0)),
    OP1(VRT,  FNC(echo_test)),

    OP0(END)
  };

  vm_exec(&vm, ops);

  printf(
    "vm.sp = %lu\nvm.bp = %lu\nvm.ep = %lu\n"
    "bm.cx = %ld\nvm.r0 = %lu\nvm.r1 = %lu\n\n",
    vm.sp, vm.bp, vm.ep, 
    vm.cx, vm.r0, vm.r1
  );

  printf("%lu\n", vm.r0 * 100 + vm.r1);
  
  return 0;
}
