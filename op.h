#pragma once

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

#pragma push_macro(MATH_BINOP)
#pragma push_macro(MATH_UNYOP)

#define MATH_BINOP(O,A,B)
#define MATH_UNYOP(O,A)

/**
 * handler for the SUB opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_sub (OP_ARG2)
{
  MATH_BINOP(-, arg1, arg2);
}

/**
 * handler for the ADD opcode
 * 
 * @param the virtual machine struct
 * @param the first argument (destination)
 * @param the second argument (source)
 */
extern inline OP_FUNC op_add (OP_ARG2)
{}

#pragma pop_macro(MATH_UNYOP)
#pragma pop_macro(MATH_BINOP)

/** */
extern OP_FUNC op_mov (OP_ARG2);

extern OP_FUNC op_call (OP_ARG1);

extern OP_FUNC op_ret (OP_ARG0);

extern OP_FUNC op_push (OP_ARG1);

extern OP_FUNC op_cls (OP_ARG0);

extern OP_FUNC op_end (OP_ARG0);
