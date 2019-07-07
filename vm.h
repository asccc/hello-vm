#pragma once

#include "def.h"
#include "tab.h"

#include <stdbool.h>

// bytecode magic number: HVM 3/4
#define HVM_NUM 0x004d5648 
#define HVM_M32 ((3 << 24) | HVM_NUM)
#define HVM_M64 ((4 << 24) | HVM_NUM)

#define VM_CALL __attribute__((nonnull))

struct vm;
struct vm_ins;

#ifdef __x86_64
  #define VM64 1
  #define VM32 0
  #define VM_MOD ((u8) 8)
  typedef i64 vm_dsp;
#else
  #define VM64 0
  #define VM32 1
  #define VM_MOD ((u8) 4)
  typedef i32 vm_dsp;
#endif

#if VM64
  #define MSK_8  0xFFFFFFFFFFFFFF00
  #define MSK_16 0xFFFFFFFFFFFF0000
  #define MSK_32 0xFFFFFFF000000000
  #define MSK_64 0x0000000000000000
#else
  #define MSK_8  0xFFFFFF00
  #define MSK_16 0xFFFF0000
  #define MSK_32 0x00000000
  #define MSK_64 0x00000000
#endif

#define SGN_8 0x80
#define SGN_16 0x8000
#define SGN_32 0x80000000

#if VM64
  #define SGN_64 0x8000000000000000
#endif

#if VM64
  #define VM_EX64 VM_CALL
#else
  #define VM_EX64 VM_CALL __attribute__((unused))
#endif

/**
 * Opcode handler callback
 */
typedef void (*vm_oph)(
  struct vm *vm, 
  struct vm_ins *ins
);

// yes, this typedef hides a pointer
// apologies for that
typedef const char *vm_ops;

/**
 * Errorcodes
 */
enum vm_err {
  VM_ENONE = 0,
  VM_EMOD, // invalid vm mode
  VM_EHLT, // vm was halted
  VM_EINS, // opcode error
  VM_EOPH, // opcode handler not found
  VM_ECHK, // opcode check failed
  VM_EINMR, // invalid modr/m operand
  VM_EIPSZ, // address size mismatch
  VM_EIPBZ, // address is below zero
  VM_EUNKR, // access to unknown register
  VM_EMBND, // access to memory out of bounds
  VM_EDIV0, // division by 0
  VM_EUNKI, // unknown interrupt
  VM_EJMPB, // jump or call out of bounds
  VM_EDSP,  // error reading displacemnt
};

/**
 * Error messages
 */
#define VM_ESTR_INS "unknown instruction"
#define VM_ESTR_IRM "only one argument can use memory-addressing"
#define VM_ESTR_HVM "invalid or unrecognized bytecode format"
#define VM_ESTR_OPM "invalid or missing bytecode-mode header"

/**
 * Opcodes
 */
enum vm_opc {
  OPC_NOP = 1,
  OPC_HLT,

  OPC_ADD_RM8_R8,
  OPC_ADD_RM8_IMM8,
  OPC_ADD_R8_RM8,
  OPC_ADD_RM16_R16,
  OPC_ADD_RM16_IMM16,
  OPC_ADD_R16_RM16,
  OPC_ADD_RM32_R32,
  OPC_ADD_RM32_IMM32,
  OPC_ADD_R32_RM32,
  OPC_ADD_RM64_R64,
  OPC_ADD_RM64_IMM64,
  OPC_ADD_R64_RM64,

  OPC_SUB_RM8_R8,
  OPC_SUB_RM8_IMM8,
  OPC_SUB_R8_RM8,
  OPC_SUB_RM16_R16,
  OPC_SUB_RM16_IMM16,
  OPC_SUB_R16_RM16,
  OPC_SUB_RM32_R32,
  OPC_SUB_RM32_IMM32,
  OPC_SUB_R32_RM32,
  OPC_SUB_RM64_R64,
  OPC_SUB_RM64_IMM64,
  OPC_SUB_R64_RM64,

  OPC_MUL_RM32_R32,
  OPC_MUL_RM32_IMM32,
  OPC_MUL_R32_RM32,
  OPC_MUL_RM64_R64,
  OPC_MUL_RM64_IMM64,
  OPC_MUL_R64_RM64,

  OPC_DIV_RM32_R32,
  OPC_DIV_RM32_IMM32,
  OPC_DIV_R32_RM32,
  OPC_DIV_RM64_R64,
  OPC_DIV_RM64_IMM64,
  OPC_DIV_R64_RM64,

  OPC_CMP_RM8_R8,
  OPC_CMP_RM8_IMM8,
  OPC_CMP_R8_RM8,
  OPC_CMP_RM16_R16,
  OPC_CMP_RM16_IMM16,
  OPC_CMP_R16_RM16,
  OPC_CMP_RM32_R32,
  OPC_CMP_RM32_IMM32,
  OPC_CMP_R32_RM32,
  OPC_CMP_RM64_R64,
  OPC_CMP_RM64_IMM64,
  OPC_CMP_R64_RM64,

  OPC_PUSH_R8,
  OPC_PUSH_RM8,
  OPC_PUSH_IMM8,
  OPC_PUSH_R32,
  OPC_PUSH_RM32,
  OPC_PUSH_IMM32,
  OPC_PUSH_R64,
  OPC_PUSH_RM64,
  OPC_PUSH_IMM64,

  OPC_POP_R8,
  OPC_POP_RM8,
  OPC_POP_R32,
  OPC_POP_RM32,
  OPC_POP_R64,
  OPC_POP_RM64,

  OPC_MOV_RM8_R8,
  OPC_MOV_RM8_IMM8,
  OPC_MOV_R8_RM8,
  OPC_MOV_RM16_R16,
  OPC_MOV_RM16_IMM16,
  OPC_MOV_R16_RM16,
  OPC_MOV_RM32_R32,
  OPC_MOV_RM32_IMM32,
  OPC_MOV_R32_RM32,
  OPC_MOV_RM64_R64,
  OPC_MOV_RM64_IMM64,
  OPC_MOV_R64_RM64,

  OPC_LEA_R32_M32,
  OPC_LEA_R64_M64,

  OPC_INT,
  OPC_CALL,
  OPC_RET,
  OPC_JMP,
  OPC_JZ,
  OPC_JNZ,
  OPC_JB,
  OPC_JNB,
  OPC_JBE,
  OPC_JA,

  // ------------------------
  // do not add opcodes below
  NUM_OPS
};

/**
 * Opcode mode
 */
enum vm_mod {
  MOD_NONE = 0,
  MOD_BYTE = 1,
  MOD_DWORD = 3,
  MOD_QWORD = 4,
};

/**
 * Argument type
 */
enum vm_aty {
  ARG_NIL = 0,
  ARG_IRM, // memory addressing
  ARG_REG, // register addressing
  ARG_IMM  // immediate
};

/** 
 * Bytecode extra 
 */
enum vm_ext {
  EXT_END = 0,
  EXT_DSP,
};

/**
 * Opcode argument
 */
struct vm_arg {
  enum vm_aty type;
  enum vm_ext ext;
  u8 reg;
};

/**
 * A complete instruction
 */
struct vm_ins {
  enum vm_mod mode;
  u16 code;
  struct vm_arg args[2];
};

/**
 * Register identifiers
 */
enum vm_reg {
  REG_NIL,
  REG_SP,
  REG_BP,
  REG_DS,
  REG_CS,
  REG_R0,     
  REG_R1,
  REG_R2,
  REG_R3,
};

/**
 * Virtual machine struct
 */
struct vm {
  /* code pointers */
  u8 *pp;
  u8 *ip;
  u8 *ep;
  /* memory management */
  u8 *mm;
  intptr_t mn;
  intptr_t mx;
  /* runtime registers */
  intptr_t sp;
  intptr_t bp;
  intptr_t ds;
  intptr_t cs;
  /* general purpose */
  intptr_t r0;
  intptr_t r1;
  intptr_t r2;
  /* opcode handlers */
  vm_oph oph[NUM_OPS];
  vm_ops ops[NUM_OPS];
  /* flags */
  struct {
    u32 zf: 1;
    u32 cf: 1;
    u32 sf: 1;
    u32 of: 1;
  } flg;
  /* runtime / error */
  bool hlt;
  enum vm_err err;
  enum vm_mod opm;
};

/**
 * exits the vm-loop 
 * 
 * @param the virtual machine struct^
 * @param a error message
 */
extern VM_CALL void vm_exit (struct vm *, enum vm_err);

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
 * reads the requested number of bytes from
 * the current program and stores them in 
 * read-order in the given buffer
 * 
 * @param the virtual machine struct
 * @param the number of bytes to read
 * @param the buffer
 */
extern VM_CALL void vm_read (struct vm*, szt, void*);

