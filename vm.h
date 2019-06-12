#pragma once

#include "def.h"
#include "tab.h"

#include <stdbool.h>

#define VM_CALL __attribute__((nonnull))
#define VM_INTR VM_CALL void

#define VM_NAME(ID) vm_sym__ ## ID
#define VM_ATTR __attribute__((noinline, nonnull))
#define VM_FUNC(ID) VM_ATTR void VM_NAME(ID) (struct vm *vm)

#ifndef VM_STACK_SIZE
  #define VM_STACK_SIZE 4096
#endif

#ifndef VM_USE_QWORD
  #define VM_USE_QWORD defined(__x86_64)
#endif

struct vm;
struct vm_op;

#if VM_USE_QWORD
  typedef u64 vm_max;
#else
  typedef u32 vm_max;
#endif

typedef intptr_t vm_ptr;
typedef void(*vm_oph)(struct vm*, struct vm_op*);

/**
 * vm opcodes 
 */
enum vm_opc {
  OPC_NOP = 0,
  OPC_SUB,
  OPC_ADD,
  OPC_MUL,
  OPC_DIV,
  OPC_MOV,
  OPC_CALL,
  OPC_RET,
  OPC_PUSH,
  OPC_POP,
  OPC_CLS,
  OPC_END,
};

/* keep this updated! */
#define NUM_OPC 12

/**
 * vm opcode payload kind 
 */
#define OPT_UNDEF 0u
#define OPT_BYTE  1u
#define OPT_WORD  2u
#define OPT_DWORD 3u

#if VM_USE_QWORD
  #define OPT_QWORD 4u
#endif

#define OPT_PAIR(a,b) (a << 16) | b

/**
 * represents a opcode argument
 */
struct vm_arg {
  u32 type;
  union {
    u8 byte;
    u16 word;
    u32 dword;
  #if VM_USE_QWORD
    u64 qword;
  #endif
  } data;
};

/**
 * represents a full opcode instruction
 */
struct vm_op {
  enum vm_opc code;
  u32 argc;
  struct vm_arg argv[2];
};

#define VM_ZF 1 << 1

/**
 * virtual machine struct
 */
struct vm {
  u8 *bp;
  u8 *sp;
  u8 *mem;
  u32 st;
  struct vm_op *ep;
  struct vm_arg r0;
  struct vm_arg r1;
  struct vm_arg r2;
};

/**
 * sets a flag
 * 
 * @param the virtual machine struct
 * @param the flag to set
 */
extern inline VM_CALL void vm_flag (struct vm *vm, u32 flag)
{
  vm->st |= flag;
}

/**
 * virtual machine initializer
 * 
 * @param  the virtual machine struct
 */
extern VM_CALL void vm_init (struct vm *);

/**
 * cleanup virtual machine
 * 
 * @param the virtual machine struct
 */
extern VM_CALL void vm_free (struct vm *);

/**
 * virtual machine executor
 * 
 * @param  the virtual machine struct
 * @param  the opcodes to execute
 */
extern VM_CALL void vm_exec (struct vm *, struct vm_op *);

/**
 * fetches arguments from the current stack-frame
 * 
 * @param the virtual machine struct
 * @param the argument format string
 * @param ... storage variables
 * @return true on success, false on failure
 */
extern VM_CALL bool vm_args (struct vm *, const char *, ...);
