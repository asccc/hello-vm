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

#if VM_HAS_QWORD
  #define VM_EX64 VM_CALL
#else
  #define VM_EX64 VM_CALL __attribute__((unused))
#endif

typedef void (*vm_oph)(
  struct vm*, 
  struct vm_opc*
);

// yes, this typedef hides a pointer
// apologies for that
typedef const char *vm_ops;

enum vm_err {
  VM_ENONE = 0,
  VM_EHLT, // vm was halted
  VM_EOPC, // opcode error
  VM_EOPH, // opcode handler not found
  VM_ECHK, // opcode check failed
  VM_EINMR, // invalid modr/m operand
  VM_EIPSZ, // address size mismatch
  VM_EUNKR, // access to unknown register
  VM_EMBND, // access to memory out of bounds
  VM_EDIV0, // division by 0
  VM_EUNKI, // unknown interrupt
  VM_EJMPB, // jump or call out of bounds
};

enum vm_opi {
  OPI_NOP = 0,
  OPI_HLT,

  OPI_ADD_RM8_R8,
  OPI_ADD_RM8_IMM8,
  OPI_ADD_R8_RM8,
  OPI_ADD_RM16_R16,
  OPI_ADD_RM16_IMM16,
  OPI_ADD_R16_RM16,
  OPI_ADD_RM32_R32,
  OPI_ADD_RM32_IMM32,
  OPI_ADD_R32_RM32,
  OPI_ADD_RM64_R64,
  OPI_ADD_RM64_IMM64,
  OPI_ADD_R64_RM64,

  OPI_SUB_RM8_R8,
  OPI_SUB_RM8_IMM8,
  OPI_SUB_R8_RM8,
  OPI_SUB_RM16_R16,
  OPI_SUB_RM16_IMM16,
  OPI_SUB_R16_RM16,
  OPI_SUB_RM32_R32,
  OPI_SUB_RM32_IMM32,
  OPI_SUB_R32_RM32,
  OPI_SUB_RM64_R64,
  OPI_SUB_RM64_IMM64,
  OPI_SUB_R64_RM64,

  OPI_MUL_RM32_R32,
  OPI_MUL_RM32_IMM32,
  OPI_MUL_R32_RM32,
  OPI_MUL_RM64_R64,
  OPI_MUL_RM64_IMM64,
  OPI_MUL_R64_RM64,

  OPI_DIV_RM32_R32,
  OPI_DIV_RM32_IMM32,
  OPI_DIV_R32_RM32,
  OPI_DIV_RM64_R64,
  OPI_DIV_RM64_IMM64,
  OPI_DIV_R64_RM64,

  OPI_CMP_RM8_R8,
  OPI_CMP_RM8_IMM8,
  OPI_CMP_R8_RM8,
  OPI_CMP_RM16_R16,
  OPI_CMP_RM16_IMM16,
  OPI_CMP_R16_RM16,
  OPI_CMP_RM32_R32,
  OPI_CMP_RM32_IMM32,
  OPI_CMP_R32_RM32,
  OPI_CMP_RM64_R64,
  OPI_CMP_RM64_IMM64,
  OPI_CMP_R64_RM64,

  OPI_PUSH_R8,
  OPI_PUSH_RM8,
  OPI_PUSH_IMM8,
  OPI_PUSH_R16,
  OPI_PUSH_RM16,
  OPI_PUSH_IMM16,
  OPI_PUSH_R32,
  OPI_PUSH_RM32,
  OPI_PUSH_IMM32,
  OPI_PUSH_R64,
  OPI_PUSH_RM64,
  OPI_PUSH_IMM64,

  OPI_POP_R8,
  OPI_POP_RM8,
  OPI_POP_R16,
  OPI_POP_RM16,
  OPI_POP_R32,
  OPI_POP_RM32,
  OPI_POP_R64,
  OPI_POP_RM64,

  OPI_MOV_RM8_R8,
  OPI_MOV_RM8_IMM8,
  OPI_MOV_R8_RM8,
  OPI_MOV_RM16_R16,
  OPI_MOV_RM16_IMM16,
  OPI_MOV_R16_RM16,
  OPI_MOV_RM32_R32,
  OPI_MOV_RM32_IMM32,
  OPI_MOV_R32_RM32,
  OPI_MOV_RM64_R64,
  OPI_MOV_RM64_IMM64,
  OPI_MOV_R64_RM64,

  OPI_INT,
  OPI_CALL,
  OPI_RET,
  OPI_JMP,
  OPI_JZ,
  OPI_JNZ,
  OPI_JB,
  OPI_JNB,
  OPI_JBE,
  OPI_JA,

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

enum vm_aty {
  ARG_NIL = 0,
  ARG_IRM, // register + offset
  ARG_REG, // register addressing
  ARG_IMM  // immediate
};

#define EXT_END 0
#define EXT_ARG 1
#define EXT_IMM 2

struct vm_arg {
  enum vm_aty type;
  i8 reg;
  i16 off;
};

struct vm_opc {
  enum vm_mod mode;
  u16 code;
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

#define FLG_ZF  (1u << 1)
#define FLG_CF  (1u << 2)
#define FLG_SF  (1u << 3)
#define FLG_OF  (1u << 4)

/**
 * virtual machine struct
 */
struct vm {
  u32 pc;
  u32 st;
  u8 *ip;
  u8 *ep;
  u8 *mm;
  intptr_t sp;
  intptr_t bp;
  intptr_t mn;
  intptr_t mx;
  vm_oph oph[NUM_OPS];
  vm_ops ops[NUM_OPS];
  vm_max r0;
  vm_max r1;
  vm_max r2;
  bool hlt;
  enum vm_err err;
};

/**
 * exits the vm-loop 
 * 
 * @param the virtual machine struct^
 * @param a error message
 */
extern VM_CALL void vm_exit (struct vm *, enum vm_err);

/**
 * sets a flag
 * 
 * @param the virtual machine struct
 * @param the flag to set
 */
extern VM_CALL void vm_flag (struct vm *, u32, bool);

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

/**
 * reads the requested number of bytes from
 * the current program and stores them in 
 * read-order in the given buffer
 * 
 * @param the virtual machine struct
 * @param the number of bytes to read
 * @param the buffer
 */
extern VM_CALL void vm_read (struct vm*, szt, void*);

