#include "vm.h"

#include <stdio.h>

#define OP0(OP) { .kind = OPC_ ## OP, .args = {0} }
#define OP1(OP, A1) { .kind = OPC_ ## OP, .args[0] = A1 }
#define OP2(OP, A1, A2) { .kind = OPC_ ## OP, .args = {A1, A2} }

#define REG(ID) { .type = OPT_REG, .data.reg = REG_ ## ID }
#define VAL(VL) { .type = OPT_VAL, .data.val = VL }

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  struct vm_op ops[] = {
    OP2(MOV,  REG(R0), VAL(37)),
    OP1(PUSH, REG(R0)),
    OP2(MOV,  REG(R0), VAL(13)),
    OP1(POP,  REG(R1)),
    OP0(END)
  };

  vm_exec(&vm, ops);

  printf(
    "vm.sp = %lu\nvm.bp = %lu\nvm.ep = %lu\n"
    "bm.cx = %li\nvm.r0 = %lu\nvm.r1 = %lu\n\n",
    vm.sp, 
    vm.bp, 
    vm.ep, 
    vm.cx, 
    vm.r0, 
    vm.r1
  );

  printf("%lu\n", vm.r0 * 100 + vm.r1);
  
  return 0;
}
