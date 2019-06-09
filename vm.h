#pragma once

#include "def.h"
#include "tab.h"

#include <stdbool.h>

#define VM_NAME(ID) vm_sym__ ## ID
#define VM_CALL __attribute__((nonnull))
#define VM_INTR VM_CALL void
#define VM_FUNC(ID)                  \
  __attribute__((noinline, nonnull)) \
  void VM_NAME(ID) (struct vm *vm)

struct vm;
typedef void(*vm_sym)(struct vm*);

/**
 * vm opcodes 
 */
enum vm_opc {
  OPC_NOP = 0,
  OPC_STK,
  OPC_SET,
  OPC_INI,
  OPC_SND,
  OPC_EXC,
  OPC_DEL,
  OPC_END
};

/**
 * vm opcode payload kind 
 */
enum vm_opt {
  OPT_NUM = 0,
  OPT_VID,
  OPT_STR,
  OPT_TID,
  OPT_SYM
};

/**
 * represents a value variant 
 */
enum vm_var {
  VAR_NIL = 0,
  VAR_NUM,
  VAR_STR,
  VAR_SYM
};

/**
 * represents a value
 */
struct vm_val {
  enum vm_var type;
  bool temp;
  union {
    i64 num;
    char *str;
    vm_sym sym;
  } data;
};

/**
 * represents a opcode argument
 */
struct vm_arg {
  enum vm_opt type;
  union {
    u32 vid;
    char *str;
    enum vm_var tid;
    vm_sym sym;
    i64 num;
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

/**
 * represents a stack
 */
struct vm_stk {
  szt size;
  struct vm_val **data;
  struct vm_stk *prev;
};

/**
 * represents a invocation
 */
struct vm_inv {
  vm_sym sym;
  u32 argc;
  u32 indx;
  struct vm_val **argv;
  struct vm_val *retv;
};

/**
 * virtual machine struct
 */
struct vm {
  szt ep;
  struct vm_stk *stk;
  struct vm_inv *inv;
};

/**
 * virtual machine initializer
 * 
 * @param  the virtual machine struct
 */
extern VM_CALL void vm_init (struct vm *);

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
