#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef size_t szt;

typedef uint8_t  u8;
typedef int8_t   i8;
typedef uint16_t u16;
typedef int16_t  i16;
typedef uint32_t u32;
typedef int32_t  i32;
typedef uint64_t u64;
typedef int64_t  i64;

#ifdef __x86_64
  #define EX64
#else
  #define EX64 __attribute__((unused))
#endif

