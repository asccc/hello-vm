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
  #define VM_STACK_SIZE 64
#endif

#ifndef VM_USE_QWORD
  #ifdef __x86_64
    #define VM_USE_QWORD 1
  #else
    #define VM_USE_QWORD 0
  #endif
#endif

struct vm;
struct vm_opc;
struct vm_imm;

typedef intptr_t vm_ptr;

#if VM_USE_QWORD
  typedef u64 vm_max;
#else
  typedef u32 vm_max;
#endif

enum vm_opi {
  OP_NOP,
  OP_SUB = 1
};

enum vm_mod {
  MOD_BYTE = 1,
  MOD_WORD = 2,
  MOD_DWORD = 4,
#if VM_USE_QWORD
  MOD_QWORD = 8,
#endif
};

struct vm_imm {
  u8 size;
  union {
    u8 byte;
    u16 word;
    u32 dword;
  #if VM_USE_QWORD
    u64 qword;
  #endif
    intptr_t addr;
  } data;
};

enum vm_aty {
  ARG_NIL = 0,
  ARG_REG,
  ARG_PTR,
  ARG_IMM
};

#define EXT_END 0
#define EXT_ARG 1
#define EXT_IMM 2

struct vm_arg {
  enum vm_aty type;
  u32 reg;
  u32 off;
};

struct vm_opc {
  enum vm_mod mode;
  u32 code;
  struct vm_arg args[2];
};

/**
 * virtual machine struct
 */
struct vm {
  u32 pc;
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
extern VM_CALL void vm_exec (struct vm *, u8*);

/**
 * fetches arguments from the current stack-frame
 * 
 * @param the virtual machine struct
 * @param the argument format string
 * @param ... storage variables
 * @return true on success, false on failure
 */
extern VM_CALL bool vm_args (struct vm *, const char *, ...);
