#pragma once

#include "vm.h"

/* operation result */
enum op_res {
  RES_ERR = 0, /* bail out */
  RES_HLT,     /* terminate execution */
  RES_NXT,     /* continue with next instruction */
  RES_CNT      /* continue with current instruction */
};

#define OP_CALL __attribute__((nonnull))
#define OP_FUNC OP_CALL enum op_res
#define OP_ARGS struct vm *vm, struct vm_op *op
#define OP_ARGV(I) (op->argv + (I))
#define OP_TYPE(I) OP_ARGV(I)->type
#define OP_DATA(I) OP_ARGV(I)->data

#define OP_DATA_CHECK(T, I, M, O, V) \
  case T:                            \
    (O) = OP_DATA(I).M == V;         \
    break;

#define OP_DATA_CHECK_BYTE(I, O, V) \
  OP_DATA_CHECK(OPT_BYTE, I, byte, O, V)
#define OP_DATA_CHECK_WORD(I, O, V) \
  OP_DATA_CHECK(OPT_WORD, I, word, O, V)
#define OP_DATA_CHECK_DWORD(I, O, V) \
  OP_DATA_CHECK(OPT_DWORD, I, dword, O, V)

#if VM_USE_QWORD
  #define OP_DATA_CHECK_QWORD(I, O, V) \
    OP_DATA_CHECK(OPT_QWORD, I, qword, O, V)
#else
  #define OP_DATA_CHECK_QWORD(I, O, V)
#endif

#define OP_DATA_CHECK_FOR_DIV(I) {   \
  bool n = false;                    \
  switch (OP_TYPE(I)) {              \
    OP_DATA_CHECK_BYTE(I, n, 0)      \
    OP_DATA_CHECK_WORD(I, n, 0)      \
    OP_DATA_CHECK_DWORD(I, n, 0)     \
    OP_DATA_CHECK_QWORD(I, n, 0)     \
    default:                         \
      return RES_ERR;                \
  }                                  \
  if (n) {                           \
    vm_warn(vm, "division by zero"); \
    return RES_HLT;                  \
  }                                  \
}

#define UNOP_CASE(T, M, O)         \
  case T:                          \
    OP_DATA(0).M = O OP_DATA(0).M; \
    if (OP_DATA(0).M == 0) {       \
      vm_flag(vm, VM_ZF);          \
    }                              \
    break; 

#define UNOP_CASE_BYTE(O) \
  UNOP_CASE(OPT_BYTE, byte, O)
#define UNOP_CASE_WORD(O) \
  UNOP_CASE(OPT_WORD, word, O)
#define UNOP_CASE_DWORD(O) \
  UNOP_CASE(OPT_DWORD, dword, O)

#if VM_USE_QWORD
  #define UNOP_CASE_QWORD(O) \
    UNOP_CASE(OPT_QWORD, qword, O)
#else 
  #define UNOP_CASE_QWORD(O)
#endif

#define UNOP(O)         \
  switch (OP_TYPE(0)) { \
    UNOP_CASE_BYTE(O)   \
    UNOP_CASE_WORD(O)   \
    UNOP_CASE_DWORD(O)  \
    UNOP_CASE_QWORD(O)  \
    default:            \
      return RES_ERR;   \
  }                     \
  return RES_NXT;

#define BINOP_PAIR \
  OPT_PAIR(OP_TYPE(0), OP_TYPE(1))

#define BINOP_PAIR_CASE(T, M, O)  \
  case OPT_PAIR(T, T): {          \
    OP_DATA(0).M O= OP_DATA(1).M; \
    if (OP_DATA(0).M == 0) {      \
      vm_flag(vm, VM_ZF);         \
    }                             \
    break;                        \
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
      return RES_ERR;   \
  }                     \
  return RES_NXT;

/**
 * handler for the SUB opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_sub (OP_ARGS)
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
extern inline OP_FUNC op_add (OP_ARGS)
{
  BINOP(+)
}

/**
 * handler for the MUL opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_mul (OP_ARGS)
{
  BINOP(*)
}

/**
 * handler for the DIV opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_div (OP_ARGS)
{
  OP_DATA_CHECK_FOR_DIV(1)
  BINOP(/)
}

/**
 * handler for the MOD opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_mod (OP_ARGS)
{
  OP_DATA_CHECK_FOR_DIV(1)
  BINOP(%)
}

/**
 * handler for the AND opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_and (OP_ARGS)
{
  BINOP(&)
}

/**
 * handler for the OR opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_or (OP_ARGS)
{
  BINOP(|)
}

/**
 * handler for the XOR opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_xor (OP_ARGS)
{
  BINOP(^)
}

/** 
 * handler for the MOV opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_mov (OP_ARGS)
{
  BINOP(=)
}

/**
 * handler for the NEG opcode
 * 
 * @param the virtual machine struct
 * @param the argument (source + destination)
 */
extern inline OP_FUNC op_neg (OP_ARGS)
{
  UNOP(-)
}

/**
 * handler for the POS opcode
 * 
 * @param the virtual machine struct
 * @param the argument (source + destination)
 */
extern inline OP_FUNC op_pos (OP_ARGS)
{
  UNOP(+)
}

/**
 * handler for the NOT opcode
 * 
 * @param the virtual machine struct
 * @param the argument (source + destination)
 */
extern inline OP_FUNC op_not (OP_ARGS)
{
  UNOP(~)
}
