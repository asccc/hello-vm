#include "vm.h"

#include <stdlib.h>
#include <string.h>

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  u8 ops[42] = {0};
  u8 *ptr = ops;
  
  u32 op1 = 308584448; // push r0
  u32 op2 = 311738368; // pop r1
  u32 hlt = 1048576;

  memcpy(ptr + 0, &op1, 4);
  memcpy(ptr + 4, &op2, 4);
  memcpy(ptr + 8, &hlt, 4);

  vm_exec(&vm, ops, 4 + 4 + 4); 
  return 0;
}
