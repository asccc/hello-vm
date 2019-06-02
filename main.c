#include "vm.h"

#include <stdio.h>

#define OP0(OP) { .kind = OPC_ ## OP, .argc = 0, .args = {0} }
#define OP1(OP, A1) { .kind = OPC_ ## OP, .argc = 1, .args[0] = A1 }
#define OP2(OP, A1, A2) { .kind = OPC_ ## OP, .argc = 2, .args = {A1, A2} }

#define REG(ID) {        \
  .type = OPT_REG,       \
  .data.reg = REG_ ## ID \
}

#define NUM(VL) {           \
  .type = OPT_VAL,          \
  .data.val.type = VAR_NUM, \
  .data.val.intr = true,    \
  .data.val.data.num = VL   \
}

#define FNC(FN) {                  \
  .type = OPT_VAL,                 \
  .data.val.type = VAR_FNC,        \
  .data.val.intr = true,           \
  .data.val.data.fnc = VM_NAME(FN) \
}

VM_FUNC(echo_r0) 
{
  printf("R0 = %lu\n", VAL_NUM(&vm->r0));
  printf("R1 = %lu\n", VAL_NUM(&vm->r1));
}

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  /**
   * INIT NUM(0), STR("hello world")
   * INIT NUM(1), NUM("11")
   * SEND VAR(0)
   * CALL echo_var
   * SEND VAR(1)
   * CALL echo_var
   * END 
   */

  struct vm_op ops[] = {
    OP2(MOV, REG(R0), NUM(20)),
    OP2(MOV, REG(R1), NUM(2)),
    OP2(MOV, NUM(22), REG(R1)),
    // OP2(MUL, REG(R0), REG(R1)),
    // OP2(ADD, REG(R0), REG(R1)),
    OP1(VRT, FNC(echo_r0)),
    OP0(END)
  };

  vm_exec(&vm, ops); 
  return 0;
}
