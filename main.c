#include "vm.h"
#include "fn.h"

#define OP0(O) {     \
  .code = OPC_ ## O, \
  .argc = 0          \
}

#define OP1(O,L) {   \
  .code = OPC_ ## O, \
  .argc = 1,         \
  .args = {L}        \
}

#define OP2(O,L,R) { \
  .code = OPC_ ## O, \
  .argc = 2,         \
  .args = {L,R}      \
}

#define VAR(N) {   \
  .type = OPT_VAR, \
  .data.str = #N   \
}

#define TID(T) {   \
  .type = OPT_TID, \
  .data.tid = T    \
}

#define SYM(S) {         \
  .type = OPT_SYM,       \
  .data.sym = VM_NAME(S) \
}

#define STR(S) {   \
  .type = OPT_STR, \
  .data.str = S    \
}

#include <stdio.h>

int main (void)
{
  struct tab tab;
  tab_init(&tab);

  tab_put(&tab, "a", "hello", sizeof("hello"));
  tab_put(&tab, "b", "world", sizeof("world"));
  
  puts(tab_get(&tab, "a"));
  puts(tab_get(&tab, "b"));

  tab_free(&tab);

  #if 0
  struct vm vm;
  vm_init(&vm);

  struct vm_op ops[] = {
    OP2(VAL, VAR(msg),  TID(VAR_STR)),
    OP2(SET, VAR(msg),  STR("hello world")),
    OP2(INI, SYM(puts), TID(VAR_NIL)),
    OP1(SND, VAR(msg)),
    OP0(EXC),
    OP1(DEL, VAR(msg)),
    OP0(END)
  };

  vm_exec(&vm, ops); 
  #endif
  return 0;
}
