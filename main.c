#include "vm.h"

#include <stdlib.h>
#include <string.h>

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  u8 ops[42] = {0};
  u8 *ptr = ops;
  
  // mov r0, 40
  // mov r1, 2
  // add r0, r1
  // dbg
  // end

  u32 m01 = 1079590945;
  u16 m02 = 24576;

  // u32 s01 = 1083457586;
  // u64 s02 = 1;

  u32 hlt = 269484032;
  
  memcpy(ptr + 0, &m01, 4);
  memcpy(ptr + 4, &m02, 2);
  // memcpy(ptr + 6, &s01, 4);
  // memcpy(ptr + 10, &s02, 8);
  memcpy(ptr + 6, &hlt, 4);

  vm_exec(&vm, ops, 4 + 2 + 4); 
  return 0;
}
