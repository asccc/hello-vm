#!/usr/bin/env php
<?php

declare(strict_types=1);
namespace HelloVM;

const RE_CASE = '/^
  OPCODE\s*\(
  (?<code>\w+)\s*
  (?:
    ,\s*(?<args>
      (\w+(?:\s*\|\s*\w+)*)
      (?:\s*,\s*(?3))*
    )*
  )?\s*\)\s*
  \{\s*
$/x';

const RE_ARGN = '/\b_(\d+)\b/';

function gen_vm(string $src, string $dst)
{
  $out = fopen($dst, 'w+');

  foreach (file($src) as $line) {
    if (ctype_space($line) ||
        substr($line, 0, 1) === '#' ||
        substr($line, 0, 2) == '//' ||
        substr($line, 0, 6) === 'extern') {
      continue;
    }

    if (preg_match(RE_CASE, $line, $m)) {
      $code = $m['code'];
      $args = $m['args'] ?? null;

      $case = "case OPC_{$code}: {\n";
      
      if (!empty($args)) {
        $args = preg_split('/\s*,\s*/', $args);
        $argc = count($args);

        $case .= "  test_argc(\"{$code}\", op->argc == {$argc});\n";

        foreach ($args as $off => $arg) {
          $hint = preg_split('/\s*\|\s*/', $arg);
          
          $argt = "(op->argv + {$off})->type";
          $case .= "  test_argv(\"{$code}\", {$off}, ";
          $frst = true;
          $abrk = count($hint) > 1;

          foreach ($hint as $type) {
            if (!$frst) $case .= ' || ';
            if ($abrk) $case .= "\n    ";
            $frst = false;
            $case .= "{$argt} == OPT_{$type}";
          }

          if ($abrk) $case .= "\n  ";
          $case .= ");\n";
        }
      } else {
        $case .= "  test_argc(\"{$code}\", op->argc == 0);\n";
      }

      fputs($out, $case);
      continue;
    }

    if ($line === '}') {
      fputs($out, "}");
      continue;
    }

    $line = preg_replace(RE_ARGN, '(op->argv + $1)', $line);
    $line = preg_replace(
      ['/\bHALT\b/', '/\bNEXT\b/'],
      ['goto end;', 'vm->ep++; break;'],
      $line
    );

    fputs($out, "$line");
  }
}

gen_vm(
  __dir__ . '/vm.def',
  __dir__ . '/vm.inc'
);
