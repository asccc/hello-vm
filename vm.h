#pragma once

#include "def.h"

#define VM_CALL     __attribute__((nonnull))
#define VM_FUNC(ID) __attribute__((noinline, nonnull)) void ID (struct vm *vm)

#define VM_STK_MIN 0x0
#define VM_STK_MAX 0xf

/**
 * virtual machine struct
 */
struct vm {
  u64 sp;
  u64 bp;
  u64 ep;
  i64 cx;
  u64 r0;
  u64 r1;
  u64 stk[VM_STK_MAX + 1];
};

/**
 * register kind 
 */
enum vm_reg {
  REG_SP = 0,
  REG_BP,
  REG_EP,
  REG_R0,
  REG_R1
};

/**
 * vm opcodes 
 */
enum vm_opc {
  OPC_NOP = 0,
  OPC_PUSH,
  OPC_POP,
  OPC_MOV,
  OPC_CALL,
  OPC_JMP,
  OPC_CMP,
  OPC_JE,
  OPC_JL,
  OPC_JG,
  OPC_ADD,
  OPC_SUB,
  OPC_INC,
  OPC_DEC,
  OPC_RET,
  OPC_END
};

/**
 * vm opcode payload kind 
 */
enum vm_opt {
  OPT_REG = 0,
  OPT_OFF,
  OPT_VAL,
  OPT_PTR
};

/**
 * represents a offset expression
 */
struct vm_off {
  i64 val;
  enum vm_reg reg;
};

/**
 * represents a opcode argument
 */
struct vm_arg {
  enum vm_opt type;
  union {
    u64 val;
    intptr_t ptr;
    struct vm_off off;
    enum vm_reg reg;
  } data;
};

/**
 * represents a full opcode instruction
 */
struct vm_op {
  enum vm_opc kind;
  struct vm_arg args[2];
};

/**
 * virtual machine initializer
 * 
 * @param  the virtual machine struct
 */
VM_CALL void vm_init (struct vm *);

/**
 * virtual machine executor
 * 
 * @param  the virtual machine struct
 * @param  the opcodes to execute
 */
VM_CALL void vm_exec (struct vm *, struct vm_op *);
