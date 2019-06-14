case OPC_NOP:
  rs = op_nop(vm, op);
  break;
case OPC_SUB:
  rs = op_sub(vm, op);
  break;
case OPC_ADD:
  rs = op_add(vm, op);
  break;
case OPC_MUL:
  rs = op_mul(vm, op);
  break;
case OPC_DIV:
  rs = op_div(vm, op);
  break;
case OPC_MOD:
  rs = op_mod(vm, op);
  break;
case OPC_POS:
  rs = op_pos(vm, op);
  break;
case OPC_NEG:
  rs = op_neg(vm, op);
  break;
case OPC_AND:
  rs = op_and(vm, op);
  break;
case OPC_OR:
  rs = op_or(vm, op);
  break;
case OPC_XOR:
  rs = op_xor(vm, op);
  break;
case OPC_NOT:
  rs = op_not(vm, op);
  break;
case OPC_INC:
  rs = op_inc(vm, op);
  break;
case OPC_DEC:
  rs = op_dec(vm, op);
  break;
case OPC_MOV:
  rs = op_mov(vm, op);
  break;
case OPC_CALL:
  rs = op_call(vm, op);
  break;
case OPC_RET:
  rs = op_ret(vm, op);
  break;
case OPC_PUSH:
  rs = op_push(vm, op);
  break;
case OPC_POP:
  rs = op_pop(vm, op);
  break;
case OPC_CLS:
  rs = op_cls(vm, op);
  break;
case OPC_DBG:
  rs = op_dbg(vm, op);
  break;
case OPC_PNT:
  rs = op_pnt(vm, op);
  break;
case OPC_END:
  rs = op_end(vm, op);
  break;
