/* 
  ONLY vm.c CAN INCLUDE THIS FILE!
  DO NOT INCLUDE THIS HEADER SOMEWHERE ELSE!
  OTHERWISE A PORT TO THE MACRO-HELL WILL OPEN!
*/

#include "vm.h"

/* implemented in vm.c */
static inline u8   memr_8 (struct vm*, u8*);
static inline u16  memr_16 (struct vm*, u8*);
static inline u32  memr_32 (struct vm*, u8*);
static inline void memw_8 (struct vm*, u8*, u8);
static inline void memw_16 (struct vm*, u8*, u16);
static inline void memw_32 (struct vm*, u8*, u32);

#if VM_USE_QWORD
  /* implemented in vm.c */
  static inline u64  memr_64 (struct vm*, u8*);
  static inline void memw_64 (struct vm*, u8*, u64);
#endif

#ifndef NDEBUG         
  #include <stdio.h>
#endif

/* operation result */
enum op_res {
  RES_ERR = 0,
  RES_HLT,
  RES_END,
  RES_NXT,
  RES_CNT
};

#define OP_CALL __attribute__((nonnull))
#define OP_FUNC OP_CALL enum op_res

#define OP_ARGS struct vm *vm, struct vm_op *op
#define OP_ARGV(I) (op->argv + (I))

#define OP_TYPE(I) OP_ARGV(I)->type
#define OP_DATA(I) OP_ARGV(I)->data
#define OP_PNTR(I) OP_DATA(I).pntr

#define OP_ADDR(I) ((vm_ptr)*(OP_DATA(I).addr))
#define OP_ADDL(I) (*(OP_DATA(I).addr))

#define OP_FLAG(I) OP_ARGV(I)->flag
#define OP_ALIT(I) (OP_FLAG(I) & ARG_LIT)
#define OP_APTR(I) (OP_FLAG(I) & ARG_PTR)
#define OP_AADR(I) (OP_FLAG(I) & ARG_ADR)

#define OP_DEST(I) (OP_APTR(I) || OP_AADR(I))

#define OP_RBIT(I,S)   memr_## S (vm, OP_PNTR(I)) 
#define OP_WBIT(I,S,V) memw_## S (vm, OP_PNTR(I), V)

#define OP_DECL(S,N) u ## S N

#define OP_FCHK             \
  if (vm_fchk(vm, VM_EF)) { \
    return RES_ERR;         \
  }

#define OP_FLGZ(V)      \
  if ((V) == 0) {       \
    vm_flag(vm, VM_EF); \
  }

#define OP_DATA_CHECK(T, I, M, O, V) \
  case T:                            \
    if (OP_APTR(I)) {                \
      (O) = OP_RBIT(I, T) == V;      \
      OP_FCHK                        \
      break;                         \
    }                                \
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

#define UNOP_CASE(T, M, O)            \
  case T: {                           \
    if (!OP_DEST(0)) {                \
      vm_warn(vm, "unused result");   \
      break;                          \
    }                                 \
    if (OP_APTR(0)) {                 \
      OP_DECL(T, _0) = OP_RBIT(0, T); \
      OP_FCHK                         \
      OP_FLGZ(_0 = O _0);             \
      OP_WBIT(0, T, _0);              \
      OP_FCHK                         \
      break;                          \
    }                                 \
    vm_ptr _a = OP_ADDR(0);           \
    OP_FLGZ(_a = O _a);               \
    OP_ADDL(0) = (u8*) _a;            \
    break;                            \
  }

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

#define BINOP_PAIR_CASE(T, M, O)      \
  case OPT_PAIR(T, T): {              \
    if (!OP_DEST(0)) {                \
      vm_warn(vm, "unused result");   \
      break;                          \
    }                                 \
    if (OP_APTR(0)) {                 \
      OP_DECL(T, _0) = OP_RBIT(0, T); \
      OP_FCHK                         \
      OP_DECL(T, _1);                 \
      if (OP_APTR(1)) {               \
        _1 = OP_RBIT(1, T);           \
        OP_FCHK                       \
      } else {                        \
        if (OP_AADR(1)) {             \
          _1 = OP_ADDR(1);            \
        } else {                      \
          _1 = OP_DATA(1).M;          \
        }                             \
      }                               \
      OP_FLGZ(_0 O ## = _1);          \
      OP_WBIT(0, T, _0);              \
      OP_FCHK                         \
    } else {                          \
      vm_ptr _a = OP_ADDR(0);         \
      vm_ptr _b = 0;                  \
      if (OP_APTR(1)) {               \
        _b = OP_RBIT(1, T);           \
        OP_FCHK                       \
      } else {                        \
        if (OP_AADR(1)) {             \
          _b = OP_ADDR(1);            \
        } else {                      \
          _b = OP_DATA(1).M;          \
        }                             \
      }                               \
      OP_FLGZ(_a O ## = _b);          \
      OP_ADDL(0) = (u8*) _a;          \
    }                                 \
    break;                            \
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
 * @param the virtual opcode
 */
extern inline OP_FUNC op_sub (OP_ARGS)
{
  BINOP(-)
}

/**
 * handler for the ADD opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_add (OP_ARGS)
{
  BINOP(+)
}

/**
 * handler for the MUL opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_mul (OP_ARGS)
{
  BINOP(*)
}

/**
 * handler for the DIV opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
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
 * @param the virtual opcode
 */
extern inline OP_FUNC op_mod (OP_ARGS)
{
  OP_DATA_CHECK_FOR_DIV(1)
  BINOP(%)
}

/**
 * handler for the SHL opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_shl (OP_ARGS)
{
  BINOP(<<);
}

/**
 * handler for the SHR opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_shr (OP_ARGS)
{
  BINOP(>>);
}

/**
 * handler for the AND opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_and (OP_ARGS)
{
  BINOP(&)
}

/**
 * handler for the OR opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_or (OP_ARGS)
{
  BINOP(|)
}

/**
 * handler for the XOR opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_xor (OP_ARGS)
{
  BINOP(^)
}

/** 
 * handler for the MOV opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_mov (OP_ARGS)
{
  BINOP() /* without operator, the op gets decoded to a single "=" */
}

/**
 * handler for the NEG opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_neg (OP_ARGS)
{
  UNOP(-)
}

/**
 * handler for the POS opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_pos (OP_ARGS)
{
  UNOP(+)
}

/**
 * handler for the NOT opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_not (OP_ARGS)
{
  UNOP(~)
}

/**
 * handler for the NOP opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_nop (OP_ARGS)
{
  return RES_NXT;
}

extern inline OP_FUNC op_call (OP_ARGS)
{
  return RES_HLT;
}

extern inline OP_FUNC op_ret (OP_ARGS)
{
  return RES_HLT;
}

extern inline OP_FUNC op_push (OP_ARGS)
{
  return RES_HLT;
}

extern inline OP_FUNC op_pop (OP_ARGS)
{
  return RES_HLT;
}

extern inline OP_FUNC op_end (OP_ARGS)
{
  return RES_END;
}

extern inline OP_FUNC op_cls (OP_ARGS)
{
  return RES_HLT;
}

/**
 * handler for the DBG opcode
 * 
 * @param the virtual machine struct
 * @param the virtual opcode
 */
extern inline OP_FUNC op_dbg (OP_ARGS)
{
#ifdef NDEBUG
  /* DBG is a NOP in non-debug builds */
  return RES_NXT;
#else

  #if VM_USE_QWORD
    #ifdef PRIu64
      #define FMT_REG "%" PRIu64
    #else
      #ifndef _WIN32
        #define FMT_REG "%lu"
      #else
        #define FMT_REG "%llu"
      #endif
    #endif
  #else
    #define FMT_REG "%lu"
  #endif

  printf("BP = %p\n", (void *) vm->bp);
  printf("SP = %p\n", (void *) vm->sp);
  printf("EP = %p\n", (void *) vm->ep);
  printf("R0 = { " FMT_REG " }\n", vm->r0.value);
  printf("R1 = { " FMT_REG " }\n", vm->r1.value);
  printf("R2 = { " FMT_REG " }\n", vm->r2.value);
  printf("ST = ");

  u32 bit = 0x80000000ul;
  do {
    putc((vm->st & bit) ? '1' : '0', stdout);
    bit >>= 1;
  } while (bit > 0);

  puts("");

  return RES_NXT;

#endif
}
