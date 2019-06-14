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
  #ifdef __x86_64
    #define VM_USE_QWORD 1
  #else
    #define VM_USE_QWORD 0
  #endif
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
  OPC_MOD,
  OPC_POS,
  OPC_NEG,
  OPC_AND,
  OPC_OR,
  OPC_XOR,
  OPC_NOT,
  OPC_MOV,
  OPC_CALL,
  OPC_RET,
  OPC_PUSH,
  OPC_POP,
  OPC_CLS,
  OPC_DBG,
  OPC_END,
};

/**
 * vm opcode payload kind 
 */
#define OPT_UNDEF 0
#define OPT_BYTE  8
#define OPT_WORD  16
#define OPT_DWORD 32

#if VM_USE_QWORD
  #define OPT_QWORD 64
#endif

#define OPT_ADDR -1
#define OPT_PAIR(A,B) ((A << 16) | B)

/**
 * represents a value
 */
union vm_val {
  /* literal values */
  u8 byte;
  u16 word;
  u32 dword;

#if VM_USE_QWORD
  u64 qword;
#endif

#ifndef NDEBUG
  intptr_t value;
#endif

  u8  *pntr;
  u8 **addr;
};

#define ARG_LIT (1u << 0)
#define ARG_PTR (1u << 1)
#define ARG_ADR (1u << 2)

/**
 * represents a opcode argument
 */
struct vm_arg {
  u32 type;
  u32 flag;
  union vm_val data;
};

/**
 * represents a full opcode instruction
 */
struct vm_op {
  enum vm_opc code;
  u32 argc;
  struct vm_arg argv[2];
};

#define VM_EF (1u << 0)
#define VM_ZF (1u << 1)

/**
 * virtual machine struct
 */
struct vm {
  u32 st;
  struct vm_op *ep;
  union vm_val r0;
  union vm_val r1;
  union vm_val r2;
  u8 *bp;
  u8 *sp;
  u8 *mem;
};

/**
 * sets a flag
 * 
 * @param the virtual machine struct
 * @param the flag to set
 */
extern VM_CALL void vm_flag (struct vm *, u32);

/**
 * checks if a flag is set
 *
 * @param the virtual machine struct
 * @param the flag to check
 * @return true if set, false otherwise
 */
extern VM_CALL bool vm_fchk (struct vm *, u32);

/**
 * emits a warning message in the vm-context
 * 
 * @param the virtual machine struct
 * @param the warning message
 */
extern VM_CALL void vm_warn (struct vm *, const char *);

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
