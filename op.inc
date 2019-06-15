#pragma once

/**
 * this file implements all opcodes
 * for excessive inlining into the vm
 */

#include "op.h"

#define OP_IMPL extern inline __attribute__((gnu_inline)) OP_FUNC
#include "op.c"

