#include "vm.h"

#include <stdlib.h>
#include <string.h>

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  u8 ops[42] = {0};
  u8 *ptr = ops;
  
  u32 add1 = 270835721;
  u16 add2 = 24576;

  u32 hlt = 1048576;

  memcpy(ptr + 0, &add1, 4);
  memcpy(ptr + 4, &add2, 2);
  memcpy(ptr + 6, &hlt, 4);

  vm_exec(&vm, ops, 4 + 2 + 4); 
  return 0;
}
