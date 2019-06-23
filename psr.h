#pragma once

#include "vm.h"
#include "def.h"
#include "tab.h"

#include <stdio.h>

#define PSR_CALL __attribute__((nonnull))

enum psr_tty {
  T_NONE = 0,
  T_LBR = '[',
  T_RBR = ']',
  T_COMMA = ',',
  T_SEMI = ';',
  T_MODE = 256,
  T_EOF,
  T_FUNC,
  T_NAME,
  T_NUM,
  T_STR,
};

struct psr_tok {
  enum psr_tty type;
  union {
    char *sym;
    u64 *num;
  } data;
};

struct psr {
  struct tab sym_tab;
  struct tab jmp_tab;
  struct psr_tok tok;
  enum vm_mod mode;
  FILE *file;
  u8 *code;
  u32 size;
};

/**
 * initializes the parser
 * 
 * @param the parser struct
 */
extern PSR_CALL void psr_init (struct psr*);

/**
 * parses a file-path
 * 
 * @param the parser struct
 * @param a path to a file
 * @return true on success, false on failure
 */
extern PSR_CALL bool psr_path (struct psr*, const char*);

/**
 * parses a file
 * 
 * @param the parser struct
 * @param a file pointer
 * @return true on success, false on failure
 */
extern PSR_CALL bool psr_file (struct psr*, FILE*);

/**
 * executes the parsed code in the vm
 * 
 * @param the parser struct
 * @param the virtual machine
 */
extern PSR_CALL void psr_exec (struct psr*, struct vm*);

/**
 * releases the parser
 * 
 * @param the parser struct
 */
extern PSR_CALL void psr_free (struct psr*);
