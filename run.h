#pragma once

#include "vm.h"
#include "def.h"

#define RUN_CALL __attribute__((nonnull))

#define RUN_ARGS      \
  struct vm *vm,      \
  struct vm_arg *arg

/**
 * ModR/M API
 */

extern RUN_CALL void mrmw_8 (RUN_ARGS, u8);
extern RUN_CALL u8   mrmr_8 (RUN_ARGS);

/**
 * Register API
 */

extern RUN_CALL void regw_8 (RUN_ARGS, u8);
extern RUN_CALL u8   regr_8 (RUN_ARGS);

/**
 * Immediate API
 */

extern RUN_CALL u8 immr_8 (struct vm*);
