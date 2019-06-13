#!/usr/bin/env php
<?php

declare(strict_types=1);
namespace HelloVM;

const OPS = [
    'NOP', 'SUB', 'ADD', 'MUL', 'DIV',
    'MOD', 'POS', 'NEG', 'AND', 'OR',
    'XOR', 'NOT', 'MOV', 'CALL', 'RET',
    'PUSH', 'POP', 'CLS', 'DBG', 'END',
];

$out = fopen(__dir__ . '/vm.inc', 'w+');
foreach (OPS as $op) {
    $u = strtoupper($op);
    $l = strtolower($op);
    fwrite($out, "case OPC_{$u}:\n  rs = op_{$l}(vm, op);\n  break;\n");
}
fclose($out);
