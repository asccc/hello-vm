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

#if VM_USE_QWORD
  typedef u64 vm_max;
  #define MSK_8  0x00000000000000FF
  #define MSK_16 0x000000000000FFFF
  #define MSK_32 0x00000000FFFFFFFF
  #define MSK_64 0xFFFFFFFFFFFFFFFF
#else
  typedef u32 vm_max;
  #define MSK_8  0x000000FF
  #define MSK_16 0x0000FFFF
  #define MSK_32 0xFFFFFFFF
  #define MSK_64 0xFFFFFFFF
#endif

#if VM_USE_QWORD || defined(__x86_64)
  #define VM_HAS_QWORD 1
#else
  #define VM_HAS_QWORD 0
#endif

typedef void (*vm_oph)(
  struct vm*, 
  struct vm_opc*, 
  struct vm_imm*
);

enum vm_opi {
  OP_NOP = 0,
  OP_HLT,

  OP_ADD_M8,
  OP_ADD_M16,
  OP_ADD_M32,
  OP_ADD_M64,

  OP_SUB_M8,
  OP_SUB_M16,
  OP_SUB_M32,
  OP_SUB_M64,

  // ------------------------
  // do not add opcodes below
  NUM_OPS
};

enum vm_mod {
  MOD_BYTE = 1,
  MOD_WORD = 2,
  MOD_DWORD = 3,
  MOD_QWORD = 4,
};

struct vm_imm {
  u8 size;
  union {
    u8 byte;
    u16 word;
    u32 dword;
  #if VM_HAS_QWORD
    u64 qword;
  #endif
  } data;
};

#ifdef __x86_64
  #define IMM_P(i) (i).data.qword
  #define IMM_PP(i) (i)->data.qword
#else
  #define IMM_P(i) (i).data.dword
  #define IMM_PP(i) (i)->data.dword
#endif

enum vm_aty {
  ARG_NIL = 0,
  ARG_ADR,
  ARG_PTR,
  ARG_IMM
};

#define EXT_END 0
#define EXT_ARG 1
#define EXT_IMM 2

struct vm_arg {
  enum vm_aty type;
  i8 reg;
  i8 off;
};

struct vm_opc {
  enum vm_mod mode;
  u32 code;
  struct vm_arg args[2];
};

enum vm_reg {
  REG_SP = 1,
  REG_BP,
  REG_DS,
  REG_CS,
  REG_R0,
  REG_R1,
  REG_R2
};

#define FLG_HLT (1u << 0)
#define FLG_Z   (1u << 1)
#define FLG_C   (1u << 2)

/**
 * virtual machine struct
 */
struct vm {
  u32 pc;
  u32 st;
  u8 *ip;
  u8 *ep;
  u8 *mm;
  u8 *sp;
  u8 *bp;
  intptr_t mn;
  intptr_t mx;
  vm_oph oph[NUM_OPS];
  char  *ops[NUM_OPS];
  vm_max r0;
  vm_max r1;
  vm_max r2;
};

/*
    R8–R15 are the new 64-bit registers.
    R8D–R15D are the lowermost 32 bits of each register.
    R8W–R15W are the lowermost 16 bits of each register.
    R8B–R15B are the lowermost 8 bits of each register.
*/

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
 * @param  the number of bytes to execute
 */
extern VM_CALL void vm_exec (struct vm *, u8*, szt);

/**
 * fetches arguments from the current stack-frame
 * 
 * @param the virtual machine struct
 * @param the argument format string
 * @param ... storage variables
 * @return true on success, false on failure
 */
extern VM_CALL bool vm_args (struct vm *, const char *, ...);
