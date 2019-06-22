#!/usr/bin/env php
<?php

declare(strict_types=1);
namespace HelloVM;

const RE_CODE = '/^\s*(OP_[a-zA-Z_0-9]+)(?:\s*=[^,]+)?,?$/';

function gen_tab (string $i, string $o)
{
  $out = fopen($o, 'w+');

  foreach (file($i) as $line) {
    if (preg_match(RE_CODE, $line, $m)) {
      switch ($m[1]) {
        case 'OP_NOP':
        case 'OP_HLT':
          break;
        default:
          $opc = $m[1];
          $oph = strtolower($opc);
          fwrite($out, "DEFN_OP($opc, $oph, \"$oph\");\n");
      }
    }
  }

  fclose($out);
}

gen_tab(
  __dir__ . '/vm.h',
  __dir__ . '/vm.tab'
);
