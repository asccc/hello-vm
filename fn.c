#include "vm.h"

#include <stdio.h>

VM_FUNC(puts) {
  char *msg = 0;
  
  if (!vm_args(vm, "s", &msg)) {
    return;
  }

  puts(msg);
}
