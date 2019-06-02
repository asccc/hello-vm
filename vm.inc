case OPC_NOP: {
  puts("NOP");
  if (op->argc != 0) {
    puts("invalid number of arguments");
    abort();
    break;
  }
  vm->ep++;
  break;
}
case OPC_MOV: {
  puts("MOV");
  if (op->argc != 2) {
    puts("invalid number of arguments");
    abort();
    break;
  }
  copy_arg(vm, op->args + 0, op->args + 1);
  vm->ep++;
  break;
}
case OPC_VRT: {
  puts("VRT");
  if (op->argc != 1) {
    puts("invalid number of arguments");
    abort();
    break;
  }
  call_arg(vm, op->args + 0);
  vm->ep++;
  break;
}
case OPC_END: {
  puts("END");
  if (op->argc != 0) {
    puts("invalid number of arguments");
    abort();
    break;
  }
  goto end;
}
