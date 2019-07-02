#include "vm.h"
#include "int.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void code1 (u8 **p,  u8 n) 
{ memcpy(*p, &n, 1); *p += 1; }

void code2 (u8 **p, u16 n) 
{ memcpy(*p, &n, 2); *p += 2; }

void code4 (u8 **p, u32 n) 
{ memcpy(*p, &n, 4); *p += 4; }

void code8 (u8 **p, u64 n) 
{ memcpy(*p, &n, 8); *p += 8; }

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  u8 ops[128] = {0};
  u8 *ptr = ops;

  code4(&ptr, HVM_M64); // 64bit header
  code4(&ptr, 1074695360);
  code2(&ptr, 0);
  code4(&ptr, 262144);  // hlt  

  vm_exec(&vm, ops, ptr - ops);

  INT_NAME(dump)(&vm);

  vm_free(&vm);

  return 0;
}
