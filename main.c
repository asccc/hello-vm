#include "vm.h"

#include <stdlib.h>
#include <string.h>

void code2 (u8 **p, u16 n) { memcpy(*p, &n, 2); *p += 2; }
void code4 (u8 **p, u32 n) { memcpy(*p, &n, 4); *p += 4; }
void code8 (u8 **p, u64 n) { memcpy(*p, &n, 8); *p += 8; }

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  u8 ops[128] = {0};
  u8 *ptr = ops;

  // main:
  code4(&ptr, 1135886336);          // push bp
  code4(&ptr, 1158955017);          // mov bp, sp
  code2(&ptr, 4096);                //         ^
  code4(&ptr, 1099177996);          // sub sp, imm64
  code8(&ptr, 16);                  //         ^
  code4(&ptr, 1157767184);          // mov [sp+0], imm64
  code8(&ptr, 6278066737626506568); //             ^
  code4(&ptr, 1157767440);          // mov [sp+8], imm64
  code8(&ptr, 143418749551);        //             ^
  code4(&ptr, 1157947404);          // mov r2, imm64
  code8(&ptr, 12);                  //         ^
  code4(&ptr, 1160773632);          // call echo
  code4(&ptr, 86);                  //      ^
  code4(&ptr, 1086595084);          // add sp, imm64
  code8(&ptr, 16);                  //         ^
  code4(&ptr, 1145323520);          // pop bp
  code4(&ptr, 1048576);             // hlt

  // echo:
  code4(&ptr, 1135886336);          // push bp
  code4(&ptr, 1158955017);          // mov bp, sp
  code2(&ptr, 4096);                //         ^
  code4(&ptr, 1158987785);          // mov r1, sp
  code2(&ptr, 4096);                //         ^
  code4(&ptr, 1087684617);          // add r1, r2
  code2(&ptr, 28672);               //         ^
  code4(&ptr, 1159725056);          // int puts
  code4(&ptr, 2);                   //     ^
  code4(&ptr, 1145323520);          // pop bp
  code4(&ptr, 1161822208);          // ret

  vm_exec(&vm, ops, ptr - ops); 
  return 0;
}
