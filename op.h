#pragma once

#include "vm.h"

#include <stdlib.h>

#define OP_CALL __attribute__((nonnull))
#define OP_FUNC OP_CALL void

/* no arguments */
#define OP_ARG0 struct vm *vm

/* one argument */
#define OP_ARG1 struct vm *vm, \
                struct vm_arg *arg1

/* two arguments */
#define OP_ARG2 struct vm *vm, \
                struct vm_arg *arg1, \
                struct vm_arg *arg2

#define UNOP(O)

#define BINOP_PAIR \
  OPT_PAIR(arg1->type, arg2->type)

#define BINOP_PAIR_CASE(T, M, O) \
  case OPT_PAIR(T, T): {         \
    arg1->data.M O arg2->data.M; \
    if (arg1->data.M == 0) {     \
      vm_flag(vm, VM_ZF);        \
    }                            \
    break;                       \
  }

#define BINOP_PAIR_BYTE(O) \
  BINOP_PAIR_CASE(OPT_BYTE, byte, O)

#define BINOP_PAIR_WORD(O) \
  BINOP_PAIR_CASE(OPT_WORD, word, O)

#define BINOP_PAIR_DWORD(O) \
  BINOP_PAIR_CASE(OPT_DWORD, dword, O)

#if VM_USE_QWORD
  #define BINOP_PAIR_QWORD(O) \
    BINOP_PAIR_CASE(OPT_QWORD, qword, O)
#else
  #define BINOP_PAIR_QWORD(O)
#endif

#define BINOP(O)        \
  switch (BINOP_PAIR) { \
    BINOP_PAIR_BYTE(O)  \
    BINOP_PAIR_WORD(O)  \
    BINOP_PAIR_DWORD(O) \
    BINOP_PAIR_QWORD(O) \
    default:            \
      abort();          \
  }

/**
 * handler for the SUB opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_sub (OP_ARG2)
{
  BINOP(-)
}

/**
 * handler for the ADD opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_add (OP_ARG2)
{
  BINOP(+)
}

/** 
 * handler for the MOV opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_mov (OP_ARG2)
{
  BINOP(=)
}

extern OP_FUNC op_call (OP_ARG1);

extern OP_FUNC op_ret (OP_ARG0);

extern OP_FUNC op_push (OP_ARG1);

extern OP_FUNC op_cls (OP_ARG0);

extern OP_FUNC op_end (OP_ARG0);
