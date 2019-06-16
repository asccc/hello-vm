#include "vm.h"

#include <stdlib.h>
#include <string.h>

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  u8 ops[42] = {0};
  u8 *ptr = ops;
  u32 opc = 2149859378;
  u64 imm = 42;
  
  memcpy(ptr + 0, &opc, 4);
  memcpy(ptr + 4, &imm, 8);

  vm_exec(&vm, ops); 
  return 0;
}
