#include "flg.h"

/**
 * {@inheritdoc}
 */
FL_CALL calc_zf (struct vm *vm, bool set)
{
  vm_flag(vm, FLG_ZF, set);
}

/**
 * {@inheritdoc}
 */
FL_CALL calc_cf (struct vm *vm, bool set)
{
  vm_flag(vm, FLG_CF, set);
}

#define SGN_8 0x80
#define SGN_16 0x8000
#define SGN_32 0x80000000

#define CALC_SF(N,M)              \
  FL_CALL calc_sf_ ## N (         \
    struct vm *vm, u ## N val     \
  ) {                             \
    vm_flag(vm, FLG_SF, val & M); \
  }

CALC_SF(8, SGN_8)
CALC_SF(16, SGN_16)
CALC_SF(32, SGN_32)

#if VM_HAS_QWORD
  #define SGN_64 0x8000000000000000
  CALC_SF(64, SGN_64)
#endif

// http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html

#define CALC_OF(N,L,U)                                     \
  FL_CALL calc_of_ ## N (                                  \
    struct vm *vm, u ## N res, u ## N v1, u ## N v2        \
  ) {                                                      \
    vm_flag(vm, FLG_OF, !((v1^v2) & L) && ((v1^res) & U)); \
  }

CALC_OF(8, SGN_8, SGN_8)
CALC_OF(16, SGN_8, SGN_16)
CALC_OF(32, SGN_8, SGN_32)

#if VM_HAS_QWORD
  CALC_OF(64, SGN_8, SGN_64)
#endif
