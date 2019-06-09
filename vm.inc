case OPC_NOP: {
  test_argc("NOP", op->argc == 0);
  vm->ep++; break;
}
case OPC_STK: {
  test_argc("STK", op->argc == 1);
  test_argv("STK", 0, (op->argv + 0)->type == OPT_NUM);
  init_stk(vm, (op->argv + 0)->data.num);
  vm->ep++; break;
}
case OPC_SET: {
  test_argc("SET", op->argc == 2);
  test_argv("SET", 0, (op->argv + 0)->type == OPT_VID);
  test_argv("SET", 1, 
    (op->argv + 1)->type == OPT_NUM || 
    (op->argv + 1)->type == OPT_STR || 
    (op->argv + 1)->type == OPT_SYM
  );
  updt_var(vm, (op->argv + 0)->data.vid, (op->argv + 1));
  vm->ep++; break;
}
case OPC_INI: {
  test_argc("INI", op->argc == 2);
  test_argv("INI", 0, 
    (op->argv + 0)->type == OPT_SYM || 
    (op->argv + 0)->type == OPT_VID
  );
  test_argv("INI", 1, (op->argv + 1)->type == OPT_NUM);
  init_inv(vm, (op->argv + 0), (op->argv + 1)->data.num);
  vm->ep++; break;
}
case OPC_SND: {
  test_argc("SND", op->argc == 1);
  test_argv("SND", 0, 
    (op->argv + 0)->type == OPT_VID || 
    (op->argv + 0)->type == OPT_NUM || 
    (op->argv + 0)->type == OPT_SYM || 
    (op->argv + 0)->type == OPT_STR
  );
  if ((op->argv + 0)->type == OPT_VID) {
    send_var(vm, (op->argv + 0)->data.vid);
    vm->ep++; break;
  }
  send_val(vm, (op->argv + 0));
  vm->ep++; break;
}
case OPC_EXC: {
  test_argc("EXC", op->argc == 0);
  exec_inv(vm);
  vm->ep++; break;
}
case OPC_DEL: {
  test_argc("DEL", op->argc == 1);
  test_argv("DEL", 0, (op->argv + 0)->type == OPT_VID);
  free_var(vm, (op->argv + 0)->data.vid);
  vm->ep++; break;
}
case OPC_END: {
  test_argc("END", op->argc == 0);
  free_stk(vm);
  goto end;
}
