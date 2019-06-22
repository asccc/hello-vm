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
};

enum vm_opi {
  OP_NOP = 0,
  OP_HLT,

  OP_ADD_RM8_R8,
  OP_ADD_RM8_IMM8,
  OP_ADD_R8_RM8,

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
