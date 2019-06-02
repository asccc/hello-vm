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

#define STR(CA) {                       \
  .type = OPT_VAL,                      \
  .data.val.type = VAR_STR,             \
  .data.val.intr = true,                \
  .data.val.data.str.data = CA,         \
  .data.val.data.str.size = sizeof(CA), \
  .data.val.data.str.buff = 0           \
}

VM_FUNC(echo_r0) 
{
  puts(VAL_STR(&vm->r0));
}

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  struct vm_op ops[] = {
    OP2(MOV, REG(R0), STR("hello, world!")),
    OP1(VRT, FNC(echo_r0)),
    OP0(END)
  };

  vm_exec(&vm, ops); 
  return 0;
}
