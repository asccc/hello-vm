#include "vm.h"
#include "int.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static u32 make_opc (
  enum vm_mod mod, 
  enum vm_opc opc, 
  enum vm_aty a1, 
  enum vm_reg r1,
  enum vm_aty a2,
  enum vm_reg r2,
  bool ext
) {
  u32 op = 0;
  op |= mod << 29;
  op |= opc << 17;
  op |= a1 << 14;
  op |= r1 << 9;
  op |= a2 << 6;
  op |= r2 << 1;
  op |= ext;
  return op;
}

void code1 (u8 **p,  u8 n) 
{ 
  memcpy(*p, &n, 1); 
  *p += 1;
}

void code2 (u8 **p, u16 n) 
{ 
  memcpy(*p, &n, 2); 
  *p += 2; 
}

void code4 (u8 **p, u32 n) 
{ 
  memcpy(*p, &n, 4); 
  *p += 4; 
}

void code8 (u8 **p, u64 n) 
{ 
  memcpy(*p, &n, 8); 
  *p += 8; 
}

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  u8 ops[512] = {0};
  u8 *ptr = ops;

  code4(&ptr, HVM_M64); // 64bit header
  
  // jmp main
  code4(&ptr, make_opc(MOD_QWORD, OPC_JMP, ARG_IRM, REG_NIL, ARG_NIL, REG_NIL, true));
  code8(&ptr, (intptr_t) ptr + 72);

  // echo:
  intptr_t echo_loc = (intptr_t) ptr;
  code4(&ptr, make_opc(MOD_QWORD, OPC_PUSH_R64, ARG_REG, REG_BP, ARG_NIL, REG_NIL, false)); 
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_R64_RM64, ARG_REG, REG_BP, ARG_REG, REG_SP, false)); 
  code4(&ptr, make_opc(MOD_QWORD, OPC_SUB_RM64_IMM64, ARG_REG, REG_SP, ARG_IMM, REG_NIL, false));
  code8(&ptr, 16);
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_RM64_R64, ARG_IRM, REG_BP, ARG_REG, REG_R1, true));
  code8(&ptr, -8);
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_R64_RM64, ARG_REG, REG_R1, ARG_IRM, REG_BP, true));
  code8(&ptr, -8);
  code4(&ptr, make_opc(MOD_QWORD, OPC_INT, ARG_NIL, REG_NIL, ARG_NIL, REG_NIL, false));
  code4(&ptr, IC_PUTS);
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_R64_RM64, ARG_REG, REG_SP, ARG_REG, REG_BP, false));
  code4(&ptr, make_opc(MOD_QWORD, OPC_POP_R64, ARG_REG, REG_BP, ARG_NIL, REG_NIL, false));
  code4(&ptr, make_opc(MOD_QWORD, OPC_RET, ARG_NIL, REG_NIL, ARG_NIL, REG_NIL, false));
  
  // main:
  code4(&ptr, make_opc(MOD_QWORD, OPC_PUSH_R64, ARG_REG, REG_BP, ARG_NIL, REG_NIL, false)); 
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_R64_RM64, ARG_REG, REG_BP, ARG_REG, REG_SP, false)); 
  code4(&ptr, make_opc(MOD_QWORD, OPC_SUB_RM64_IMM64, ARG_REG, REG_SP, ARG_IMM, REG_NIL, false));
  code8(&ptr, 16);
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_RM64_IMM64, ARG_IRM, REG_BP, ARG_IMM, REG_NIL, true));
  code8(&ptr, 8583909746840200552);
  code8(&ptr, -14);
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_RM32_IMM32, ARG_IRM, REG_BP, ARG_IMM, REG_NIL, true));
  code4(&ptr, 1684828783);
  code8(&ptr, -6);
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_RM16_IMM16, ARG_IRM, REG_BP, ARG_IMM, REG_NIL, true));
  code2(&ptr, 33);
  code8(&ptr, -2);
  code4(&ptr, make_opc(MOD_QWORD, OPC_LEA_R64_M64, ARG_REG, REG_R1, ARG_IRM, REG_BP, true));
  code8(&ptr, -14);
  code4(&ptr, make_opc(MOD_QWORD, OPC_CALL, ARG_IRM, REG_NIL, ARG_NIL, REG_NIL, true));
  code8(&ptr, echo_loc);
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_RM64_IMM64, ARG_REG, REG_R0, ARG_IMM, REG_NIL, false));
  code8(&ptr, 0);
  code4(&ptr, make_opc(MOD_QWORD, OPC_MOV_R64_RM64, ARG_REG, REG_SP, ARG_REG, REG_BP, false));
  code4(&ptr, make_opc(MOD_QWORD, OPC_POP_R64, ARG_REG, REG_BP, ARG_NIL, REG_NIL, false));
  code4(&ptr, make_opc(MOD_QWORD, OPC_HLT, ARG_NIL, REG_NIL, ARG_NIL, REG_NIL, false));  

  vm_exec(&vm, ops, ptr - ops);
  vm_free(&vm);

  return 0;
}
