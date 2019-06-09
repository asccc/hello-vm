#pragma once

#include "def.h"

/**
 * allocate or die tryin'
 * 
 * @param the number of bytes to allocate
 * @return the allocated memory
 */
extern void * mem_aodt (szt) __attribute__((malloc));

/**
 * creates a copy of the given string
 * 
 * @param the string to copy
 * @return the copied string
 */
extern char * mem_sdup (const char *) __attribute__((malloc));

/**
 * grow/shrink the given memory pointer
 * 
 * @param a pointer to the memory location
 * @param the new size
 */
extern void * mem_grow (void *, szt) __attribute__((malloc));
#define mem_srnk(m,s) mem_grow(m, s)

/**
 * release memory
 * 
 * @param a pointer to the memory location
 */
extern void mem_free (void*);
