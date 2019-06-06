#!/usr/bin/env php
<?php

declare(strict_types=1);
namespace HelloVM;

const DEF = __DIR__ . '/vm.def';
const OUT = __DIR__ . '/vm.inc';

assert_options(ASSERT_ACTIVE, true); 
assert_options(ASSERT_BAIL, true);

class OpCode
{
  public $name;
  public $argc;
  public $body;

  public function __construct(string $name, int $argc)
  {
    $this->name = $name;
    $this->argc = $argc;
    $this->body = [];
  }

  public function add(string $line)
  {
    $this->body[] = $line;
  }
}

const ST_HEAD = 0;
const ST_BODY = 1;

const RE_HEAD = '/^(\w+)\s*#(\d+)\s*\(\s*$/';
const RE_FUNC = '/^%(\w+)((?:\s+[$>]\d+)*)\s*$/';
const RE_INST = '/^\\/(\w+)$/';

exit(main());

function main(): int
{
  $fp = fopen(DEF, 'r');
  $st = ST_HEAD;
  $vm = [];
  $op = null;

  if (!$fp) {
    error_log("could not open vm-definition file\n");
    return 1;
  }

  for (;;) {
    if (($line = fgets($fp)) == false) {
      break;
    }

    $line = trim($line);

    switch ($st) {
      case ST_HEAD:
        if (!preg_match(RE_HEAD, $line, $m)) {
          error_log('syntax error near: ' . $line);
          goto err;
        }
        $op = new OpCode($m[1], (int)($m[2]));
        $st = ST_BODY;
        break;

      case ST_BODY:
        switch (substr($line, 0, 1)) {
          case '%': 
            $it = gen_fnc($line);
            break;

          case '/':
            $it = gen_sym($line);
            break;

          case ')':
            $vm[] = $op;
            $st = ST_HEAD;
            break 2;

          default:
            error_log('syntax error near: ' . $line);
            goto err;
        }
        foreach ($it as $ln) {
          $op->add($ln);
        }
        break;

      default:
        error_log('invalid state');
        goto err;
    }
  }

  fclose($fp);
  return wrt_out($vm);

  err:
  fclose($fp);
  return 1;
}

function gen_fnc(string $line)
{
  $ins = '';
  $tmp = [];

  if (!preg_match(RE_FUNC, $line, $m)) {
    error_log('invalid syntax near: ' . $line);
    yield null;
    return;
  }

  [, $fnc, $prm] = $m;
  $prm = trim($prm);

  if (!empty($prm)) {
    foreach (preg_split('/\s+/', $prm) as $arg) {
      $num = substr($arg, 1);
      switch (substr($arg, 0, 1)) {
        case '$':
          $ins .= ', op->args + ' . $num;
          break;
        case '>':
          $tmp[] = $num;
          $ins .= ', &t' . $num;
          break;
        default:
          assert(0);
      }
    }
  }

  if (!empty($tmp)) {
    foreach ($tmp as $num) {
      yield "struct vm_val t{$num} = {0};";
    }
  }

  yield "$fnc(vm$ins);";
}

function gen_sym(string $line)
{
  if (!preg_match(RE_INST, $line, $m)) {
    error_log('syntax error near: ' . $line);
    yield null;
    return;
  }

  [, $ins] = $m;

  switch ($ins) {
    case 'next':
      yield 'vm->ep++;';
      yield 'break;';
      break;
    case 'skip':
      yield 'break;';
      break;
    case 'halt':
      yield 'goto end;';
      break;
  }
}

function wrt_out(array $ops) 
{
  $fp = fopen(OUT, 'w+');
  if (!$fp) {
    error_log('could not open output file');
    return 1;
  }

  foreach ($ops as $op) {
    fputs($fp, 'case OPC_');
    fputs($fp, strtoupper($op->name));
    fputs($fp, ": {\n");
    fputs($fp, '  puts("');
    fputs($fp, strtoupper($op->name));
    fputs($fp, '");');
    fputs($fp, "\n");
    fputs($fp, '  if (op->argc != ');
    fputs($fp, (string) $op->argc);
    fputs($fp, ") {\n");
    fputs($fp, '    puts("invalid number of arguments");');
    fputs($fp, "\n    abort();\n    break;\n  }\n");
    foreach ($op->body as $line) {
      fputs($fp, "  $line\n");
    }
    fputs($fp, "}\n");
  }

  fclose($fp);
  return 0;
}
