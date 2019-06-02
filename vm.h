#pragma once

#include "def.h"

#include <stdbool.h>

#define VM_NAME(ID) vm_sym__ ## ID

#define VM_CALL __attribute__((nonnull))
  
#define VM_INTR VM_CALL void

#define VM_FUNC(ID)                  \
  __attribute__((noinline, nonnull)) \
  void VM_NAME(ID) (struct vm *vm)

struct vm;

/**
 * represents a register slot
 */
enum vm_reg {
  REG_R0 = 0,
  REG_R1
};

/**
 * vm opcodes 
 */
enum vm_opc {
  OPC_NOP = 0,
  // OPC_PUSH,
  // OPC_POP,
  OPC_MOV,
  OPC_VRT,
  // OPC_JMP,
  // OPC_CMP,
  OPC_END
};

/**
 * vm opcode payload kind 
 */
enum vm_opt {
  OPT_REG = 0,
  OPT_VAL,
};

/**
 * represents a string 
 */
struct vm_str {
  char *data;
  u32 size;
  u32 buff;
};

/**
 * represents a value variant 
 */
enum vm_var {
  VAR_NIL = 0,
  VAR_NUM,
  VAR_STR,
  VAR_FNC
};

/**
 * represents a value
 */
struct vm_val {
  enum vm_var type;
  bool intr;
  union {
    i64 num;
    struct vm_str str;
    void(*fnc)(struct vm*);
  } data;
};

#define VAL_NUM(v) (v)->data.num
#define VAL_STR(v) (v)->data.str.data
#define VAL_FNC(v) (v)->data.fnc

/**
 * represents a opcode argument
 */
struct vm_arg {
  enum vm_opt type;
  union {
    struct vm_val val;
    enum vm_reg reg;
  } data;
};

/**
 * represents a full opcode instruction
 */
struct vm_op {
  enum vm_opc kind;
  u32 argc;
  struct vm_arg args[2];
};

/**
 * represents a stack frame
 */
struct vm_stk {
  szt ep;
  szt size;
  szt buff;
  u32 argc;
  struct vm_val *argv;
  struct vm_val *data;
  struct vm_stk *prev;
  struct vm_stk *next;
};

/**
 * virtual machine struct
 */
struct vm {
  szt ep;
  struct vm_val r0;
  struct vm_val r1;
  struct vm_stk bp;
  struct vm_stk *sp;
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

/**
 * fetches arguments from the current stack-frame
 * 
 * @param the virtual machine struct
 * @param the argument format string
 * @param ... storage variables
 * @return true on success, false on failure
 */
VM_CALL bool vm_args (struct vm *, const char *, ...);
