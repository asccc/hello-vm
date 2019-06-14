#include "vm.h"
#include "mem.h"
#include "op.h"

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

/**
 * {@inheritdoc}
 */
VM_CALL void vm_init (struct vm *vm)
{
  assert(vm != 0);

  vm->mem = mem_aodt(VM_STACK_SIZE);
  vm->sp = vm->mem + VM_STACK_SIZE;
  vm->bp = vm->mem;
  vm->ep = 0;
  vm->st = 0;

  memset(&(vm->r0), 0, sizeof(vm->r0));
  memset(&(vm->r1), 0, sizeof(vm->r1));
  memset(&(vm->r2), 0, sizeof(vm->r2));
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_flag (struct vm *vm, u32 flag)
{
  vm->st |= flag;
}

/**
 * {@inheritdoc}
 */
VM_CALL bool vm_fchk (struct vm *vm, u32 flag)
{
  return (vm->st & flag) == flag;
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_warn (struct vm *vm, const char *msg)
{
  fputs(msg, stderr);
  fputs("\n", stderr);
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_free (struct vm *vm)
{
  assert(vm != 0);

  mem_free(vm->mem);
}

/**
 * {@inheritdoc}
 */
VM_CALL void vm_exec (struct vm *vm, struct vm_op *ops)
{
  assert(vm != 0);
  assert(ops != 0);

  enum op_res rs;

  vm->ep = ops;
  for (struct vm_op *op;;) {
    op = vm->ep;
    switch (op->code) {
      #include "vm.inc"
    }
    switch (rs) {
      case RES_NXT:
        vm->ep++;
        /* fall through */
      case RES_CNT:
        break;
      case RES_ERR:
        puts("a error occurred");
        /* fall through */
      case RES_HLT:
        puts("execution halted");
        /* fall through */
      case RES_END:
        goto end;
      default:
        assert(0);
    }
  }
  end:
  return;
}

/**
 * {@inheritdoc}
 */
VM_CALL bool vm_args (struct vm *vm, const char *fmt, ...)
{
  assert(vm != 0);
  assert(fmt != 0);

  return false;
}

/**
 * reads 8 bits from the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @return the value
 */
static inline u8 memr_8 (struct vm *vm, u8 *mp)
{
  return *mp;
}

/**
 * reads 16 bits from the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @return the value
 */
static inline u16 memr_16 (struct vm *vm, u8 *mp)
{
  return (
    (((u16)*(mp + 0)) <<  8) |
    (((u16)*(mp + 1))      )
  );
}

/**
 * reads 32 bits from the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @return the value
 */
static inline u32 memr_32 (struct vm *vm, u8 *mp)
{
  return (
    (((u32)*(mp + 0)) << 24) |
    (((u32)*(mp + 1)) << 16) |
    (((u32)*(mp + 2)) <<  8) |
    (((u32)*(mp + 3))      )
  );
}

#if VM_USE_QWORD
  /**
   * reads 64 bits from the given memory-pointer
   * 
   * @param the virtual-machine struct
   * @param the memory pointer
   * @return the value
   */
  static inline u64 memr_64 (struct vm *vm, u8 *mp)
  {
    return (
      (((u64)*(mp + 0)) << 56) |
      (((u64)*(mp + 1)) << 48) |
      (((u64)*(mp + 2)) << 40) |
      (((u64)*(mp + 3)) << 32) |
      (((u64)*(mp + 4)) << 24) |
      (((u64)*(mp + 5)) << 16) |
      (((u64)*(mp + 6)) <<  8) |
      (((u64)*(mp + 7))      )
    );
  }
#endif

/**
 * writes 8 bits to the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @param the value to write
 */
static inline void memw_8 (struct vm *vm, u8 *mp, u8 mv)
{
  *mp = mv;
}

/**
 * writes 16 bits to the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @param the value to write
 */
static inline void memw_16 (struct vm *vm, u8 *mp, u16 mv)
{
  *(mp + 0) = (mv >> 8) & 0xff;
  *(mp + 1) = (mv     ) & 0xff;
}

/**
 * writes 32 bits to the given memory-pointer
 * 
 * @param the virtual-machine struct
 * @param the memory pointer
 * @param the value to write
 */
static inline void memw_32 (struct vm *vm, u8 *mp, u32 mv)
{
  *(mp + 0) = (mv >> 24) & 0xff;
  *(mp + 1) = (mv >> 16) & 0xff;
  *(mp + 2) = (mv >>  8) & 0xff;
  *(mp + 3) = (mv      ) & 0xff;
}

#if VM_USE_QWORD
  /**
   * writes 64 bits to the given memory-pointer
   * 
   * @param the virtual-machine struct
   * @param the memory pointer
   * @param the value to write
   */
  static inline void memw_64 (struct vm *vm, u8 *mp, u64 mv)
  {
    *(mp + 0) = (mv >> 56) & 0xff;
    *(mp + 1) = (mv >> 48) & 0xff;
    *(mp + 2) = (mv >> 40) & 0xff;
    *(mp + 3) = (mv >> 32) & 0xff;
    *(mp + 4) = (mv >> 24) & 0xff;
    *(mp + 5) = (mv >> 16) & 0xff;
    *(mp + 6) = (mv >>  8) & 0xff;
    *(mp + 7) = (mv      ) & 0xff;
  }
#endif
