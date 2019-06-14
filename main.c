#include "vm.h"
#include "fn.h"

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  struct vm_op ops[] = {
    { .code = OPC_DBG },
    { 
      .code = OPC_SUB,
      .argv = {
        {
          .flag = ARG_ADR,
          .type = OPT_QWORD,
          .data.addr = &vm.sp
        },
        {
          .flag = ARG_LIT,
          .type = OPT_QWORD,
          .data.qword = 4
        }
      }
    },
    { .code = OPC_DBG },
    { .code = OPC_END }
  };

  vm_exec(&vm, ops); 
  return 0;
}
