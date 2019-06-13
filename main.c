#include "vm.h"
#include "fn.h"

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  struct vm_op ops[] = {
    { .code = OPC_DBG },
    { .code = OPC_END }
  };

  vm_exec(&vm, ops); 
  return 0;
}
