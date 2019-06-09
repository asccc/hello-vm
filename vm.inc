


case OPC_NOP: {
  assert(op->argc == 0);
  vm->ep++; break;
}

case OPC_VAR: {
  assert(op->argc == 1);
  assert((op->argv + 0)->type == OPT_VID);
  struct vm_val *val;
  val = stk_read(vm, (op->argv + 0)->data.vid);
  val_init(vm, val);
  vm->ep++; break;
}

case OPC_SET: {
  assert(op->argc == 2);
  assert((op->argv + 0)->type == OPT_VID);
  assert(
    (op->argv + 1)->type == OPT_NUM || 
    (op->argv + 1)->type == OPT_STR || 
    (op->argv + 1)->type == OPT_SYM
  );
  struct vm_val *val;
  val = stk_read(vm, (op->argv + 0)->data.vid);
  val_free(vm, val);
  switch ((op->argv + 1)->type) {
    case OPT_NUM:
      val->type = VAR_NUM;
      val->data.num = (op->argv + 1)->data.num;
      break;
    case OPT_STR:
      val->type = VAR_STR;
      val->data.str = mem_sdup((op->argv + 1)->data.str);
      break;
    case OPT_SYM:
      val->type = VAR_SYM;
      val->data.sym = (op->argv + 1)->data.sym;
      break;
    default:
      goto end;
  }
  vm->ep++; break;
}

case OPC_INI: {
  assert(op->argc == 2);
  assert(
    (op->argv + 0)->type == OPT_SYM || 
    (op->argv + 0)->type == OPT_VID
  );
  assert((op->argv + 1)->type == OPT_TID);
  vm_sym sym;
  switch ((op->argv + 0)->type) {
    case OPT_VID: {
      struct vm_val *val;
      val = stk_read(vm, (op->argv + 0)->data.vid);
      if (val->type != VAR_SYM) {
        puts("type error: symbol value expected");
        goto end;
      }
      sym = val->data.sym;
      break;
    }
    case OPT_SYM:
      sym = (op->argv + 0)->data.sym;
      break;
    default:
      goto end;
  }
  inv_make(vm, sym);
  vm->ep++; break;
}

case OPC_SND: {
  assert(op->argc == 1);
  assert(
    (op->argv + 0)->type == OPT_VID || 
    (op->argv + 0)->type == OPT_NUM || 
    (op->argv + 0)->type == OPT_SYM || 
    (op->argv + 0)->type == OPT_STR
  );
  struct vm_val *val;
  if ((op->argv + 0)->type == OPT_VID) {
    val = stk_read(vm, (op->argv + 0)->data.vid);
    inv_send(vm, val);
    vm->ep++; break;
  }
  val = val_make(vm);
  val->temp = true;
  switch ((op->argv + 0)->type) {
    case OPT_NUM: 
      val->type = VAR_NUM;
      val->data.num = (op->argv + 0)->data.num;
      break;
    case OPT_STR:
      val->type = VAR_STR;
      val->data.str = mem_sdup((op->argv + 0)->data.str);
      break;
    case OPT_SYM:
      val->type = VAR_SYM;
      val->data.sym = (op->argv + 0)->data.sym;
      break;
    default:
      goto end;
  }
  inv_send(vm, val);
  vm->ep++; break;
}

case OPC_EXC: {
  assert(op->argc == 0);
  inv_call(vm);
  vm->ep++; break;
}

case OPC_DEL: {
  assert(op->argc == 1);
  assert((op->argv + 0)->type == OPT_VID);
  struct vm_val *val;
  val = stk_read(vm, (op->argv + 0)->data.vid);
  val_free(vm, val);
  vm->ep++; break;
}

case OPC_END: {
  assert(op->argc == 0);
  goto end;
}
