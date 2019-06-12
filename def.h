#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef size_t   szt;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

#ifdef __x86_64
  typedef uint64_t u64;
#endif
 