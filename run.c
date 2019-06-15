#include "run.h"

#ifndef NDEBUG
  #include <stdio.h>
#endif

#if 0

/**
 * {@inheritdoc}
 */
RUN_CALL u8 memr_8 (struct vm *vm, u8 *mp)
{
#ifndef NDEBUG
  printf("READ AT ADDRESS %p\n", (void *) (mp + 0));
#endif

  return *mp;
}

/**
 * {@inheritdoc}
 */
RUN_CALL u16 memr_16 (struct vm *vm, u8 *mp)
{
#ifndef NDEBUG
  printf("READ AT ADDRESS %p\n", (void *) (mp + 1));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 0));
#endif

  return (
    (((u16)*(mp + 1)) <<  8) |
    (((u16)*(mp + 0))      )
  );
}

/**
 * {@inheritdoc}
 */
RUN_CALL u32 memr_32 (struct vm *vm, u8 *mp)
{
#ifndef NDEBUG
  printf("READ AT ADDRESS %p\n", (void *) (mp + 3));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 2));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 1));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 0));
#endif

  return (
    (((u32)*(mp + 3)) << 24) |
    (((u32)*(mp + 2)) << 16) |
    (((u32)*(mp + 1)) <<  8) |
    (((u32)*(mp + 0))      )
  );
}

/**
 * {@inheritdoc}
 */
RUN_CALL u64 memr_64 (struct vm *vm, u8 *mp)
{
#ifndef NDEBUG
  printf("READ AT ADDRESS %p\n", (void *) (mp + 7));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 6));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 5));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 4));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 3));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 2));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 1));
  printf("READ AT ADDRESS %p\n", (void *) (mp + 0));
#endif

  return (
    (((u64)*(mp + 7)) << 56) |
    (((u64)*(mp + 6)) << 48) |
    (((u64)*(mp + 5)) << 40) |
    (((u64)*(mp + 4)) << 32) |
    (((u64)*(mp + 3)) << 24) |
    (((u64)*(mp + 2)) << 16) |
    (((u64)*(mp + 1)) <<  8) |
    (((u64)*(mp + 0))      )
  );
}

/**
 * {@inheritdoc}
 */
RUN_CALL void memw_8 (struct vm *vm, u8 *mp, u8 mv)
{
#ifndef NDEBUG
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 0), mv);
#endif

  *mp = mv;
}

/**
 * {@inheritdoc}
 */
RUN_CALL void memw_16 (struct vm *vm, u8 *mp, u16 mv)
{  
#ifndef NDEBUG
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 0), (u8)((mv >> 8) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 1), (u8)((mv     ) & 0xff));
#endif

  *(mp + 0) = (mv >> 8) & 0xff;
  *(mp + 1) = (mv     ) & 0xff;
}

/**
 * {@inheritdoc}
 */
RUN_CALL void memw_32 (struct vm *vm, u8 *mp, u32 mv)
{
#ifndef NDEBUG
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 0), (u8)((mv >> 24) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 1), (u8)((mv >> 16) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 2), (u8)((mv >>  8) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 3), (u8)((mv      ) & 0xff));
#endif

  *(mp + 0) = (mv >> 24) & 0xff;
  *(mp + 1) = (mv >> 16) & 0xff;
  *(mp + 2) = (mv >>  8) & 0xff;
  *(mp + 3) = (mv      ) & 0xff;
}

/**
 * {@inheritdoc}
 */
RUN_CALL void memw_64 (struct vm *vm, u8 *mp, u64 mv)
{
#ifndef NDEBUG
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 0), (u8)((mv >> 56) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 1), (u8)((mv >> 48) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 2), (u8)((mv >> 40) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 3), (u8)((mv >> 32) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 4), (u8)((mv >> 24) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 5), (u8)((mv >> 16) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 6), (u8)((mv >>  8) & 0xff));
  printf("WRITE AT ADDRESS %p (%d)\n", (void *) (mp + 7), (u8)((mv      ) & 0xff));
#endif

  *(mp + 0) = (mv >> 56) & 0xff;
  *(mp + 1) = (mv >> 48) & 0xff;
  *(mp + 2) = (mv >> 40) & 0xff;
  *(mp + 3) = (mv >> 32) & 0xff;
  *(mp + 4) = (mv >> 24) & 0xff;
  *(mp + 5) = (mv >> 16) & 0xff;
  *(mp + 6) = (mv >>  8) & 0xff;
  *(mp + 7) = (mv      ) & 0xff;
}

/**
 * {@inheritdoc}
 */
RUN_CALL vm_ptr memr_ar (struct vm *vm, u8 *mp)
{
#ifdef _x86_64
  return memr_64(vm, mp);
#else
  return memr_32(vm, mp);
#endif
}

/**
 * {@inheritdoc}
 */
RUN_CALL void memw_ar (struct vm *vm, u8 *mp, vm_ptr mv)
{
#ifdef _x86_64
  memw_64(vm, mp, mv);
#else
  memw_32(vm, mp, mv);
#endif
}

#endif
