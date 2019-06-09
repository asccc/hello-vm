#include "mem.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * {@inheritdoc}
 */
void * mem_aodt (szt size)
{
  void *mem = malloc(size);
  if (!mem) {
    perror("out of memory");
    abort();
  }
  memset(mem, 0, size);
  return mem;
}

/**
 * {@inheritdoc}
 */
char * mem_sdup (const char *str)
{
  #ifdef __STRICT_ANSI__
    size_t len = strlen(str);
    void *mem = mem_aodt(len + 1);
    return memcpy(mem, str, len);
  #else
    char *dup = strdup(str);
    if (!dup) {
      perror("out of memory");
      abort();
    }
    return dup;
  #endif
}

/**
 * {@inheritdoc}
 */
void * mem_grow (void *mem, szt size)
{
  void *new = realloc(mem, size);
  if (!new) {
    perror("out of memory");
    abort();
  }
  return new;
}

/**
 * {@inheritdoc}
 */
void mem_free (void *mem)
{
  if (mem) {
    free(mem);
  }
}
