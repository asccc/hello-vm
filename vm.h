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

#ifdef __x86_64
#define VM_MASK_BYTE  0xffffffffffffff00ull
#define VM_MASK_WORD  0xffffffffffff0000ull
#define VM_MASK_DWORD 0xffffffff00000000ull
#define VM_MASK_QWORD 0x0000000000000000ull
#else
#define VM_MASK_BYTE  0xffffff00ul
#define VM_MASK_WORD  0xffff0000ul
#define VM_MASK_DWORD 0x00000000ul
#endif

struct vm;

/**
 * vm opcodes 
 */
enum vm_opc {
  OPC_NOP = 0,
  OPC_SUB,
  OPC_ADD,
  OPC_MOV,
  OPC_CALL,
  OPC_RET,
  OPC_PUSH,
  OPC_POP,
  OPC_BST,
  OPC_RST,
  OPC_CLS,
  OPC_END,
};

/**
 * vm opcode payload kind 
 */
enum vm_opt {
  OPT_UNDEF = 0,
  OPT_BYTE,
  OPT_WORD,
  OPT_DWORD,

#if VM_USE_QWORD
  OPT_QWORD,
#endif

  OPT_PTR,

  OPT_BYTE_PTR,
  OPT_WORD_PTR,
  OPT_DWORD_PTR,

#if VM_USE_QWORD
  OPT_QWORD_PTR,
#endif
};

enum vm_flg {
  FLG_RES = 1 << 0, /* unused */
  FLG_SNG = 1 << 1,
};

/**
 * represents a opcode argument
 */
struct vm_arg {
  enum vm_opt type;
  enum vm_flg flag;
  union {
    u8 byte;
    u16 word;
    u32 dword;
  #if VM_USE_QWORD
    u64 qword;
  #endif
    intptr_t ptr;
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

#define VM_CF 1 << 0
#define VM_ZF 1 << 1
#define VM_OF 1 << 2

/**
 * virtual machine struct
 */
struct vm {
  u8 *bp;
  u8 *sp;
  u8 *mem;
  u32 st;
#ifdef VM_USE_QWORD
  u64 cr;
#else
  u32 cr;
#endif
  struct vm_op *ep;
  struct vm_arg r0;
  struct vm_arg r1;
  struct vm_arg r2;
};

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
