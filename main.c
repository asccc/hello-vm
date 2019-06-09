#include "vm.h"
#include "fn.h"

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  struct vm_op ops[] = {
    { 
      .code = OPC_VAL, 
      .argc = 2, 
      .argv = {
        {
          .type = OPT_VID,
          .data.vid = 0
        },
        {
          .type = OPT_TID,
          .data.tid = VAR_STR
        }
      } 
    }, {
      .code = OPC_SET,
      .argc = 2,
      .argv = {
        {
          .type = OPT_VID,
          .data.vid = 0
        }, {
          .type = OPT_STR,
          .data.str = "hello world"
        }
      }
    }, {
      .code = OPC_INI,
      .argc = 2,
      .argv = {
        {
          .type = OPT_SYM,
          .data.sym = VM_NAME(puts)
        },
        {
          .type = OPT_TID,
          .data.tid = VAR_NIL
        }
      }
    }, {
      .code = OPC_SND,
      .argc = 1,
      .argv = {
        {
          .type = OPT_VID,
          .data.vid = 0
        }
      }
    }, {
      .code = OPC_EXC,
      .argc = 0
    }, {
      .code = OPC_DEL,
      .argc = 1,
      .argv = {
        {
          .type = OPT_VID,
          .data.vid = 0
        }
      }
    }, {
      .code = OPC_END,
      .argc = 0
    }
  };

  vm_exec(&vm, ops); 
  return 0;
}
