#include "vm.h"
#include "fn.h"

int main (void)
{
  struct vm vm;
  vm_init(&vm);

  struct vm_op ops[] = {
    { .code = OPC_DBG },
    { 
      .code = OPC_SUB,
      .argv = {
        {
          .flag = ARG_RAD,
          .type = OPT_QWORD,
          .data.reg = REG_SP
        },
        {
          .flag = ARG_LIT,
          .type = OPT_QWORD,
          .data.qword = 16
        }
      }
    },
    { .code = OPC_DBG },
    {
      .code = OPC_MOV,
      .argv = {
        {
          .flag = ARG_RPT,
          .type = OPT_QWORD,
          .data.reg = REG_SP
        },
        {
          .flag = ARG_LIT,
          .type = OPT_QWORD,
          .data.qword = 0x68656C6C6F2C2077 
        }
      }
    },
    { .code = OPC_DBG },
    { 
      .code = OPC_ADD, 
      .argv = {
        {
          .flag = ARG_RAD,
          .type = OPT_QWORD,
          .data.reg = REG_SP
        }, {
          .flag = ARG_LIT,
          .type = OPT_QWORD,
          .data.qword = 8
        }
      }
    },
    {
      .code = OPC_MOV,
      .argv = {
        {
          .flag = ARG_RPT,
          .type = OPT_QWORD,
          .data.reg = REG_SP
        },
        {
          .flag = ARG_LIT,
          .type = OPT_QWORD,
          .data.qword = 0x6F726C6421000000
        }
      }
    },
    { 
      .code = OPC_SUB, 
      .argv = {
        {
          .flag = ARG_RAD,
          .type = OPT_QWORD,
          .data.reg = REG_SP
        }, {
          .flag = ARG_LIT,
          .type = OPT_QWORD,
          .data.qword = 8
        }
      }
    },
    { .code = OPC_PNT },
    { .code = OPC_END }
  };

  vm_exec(&vm, ops); 
  return 0;
}
