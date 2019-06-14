#!/usr/bin/env php
<?php

declare(strict_types=1);
namespace HelloVM;


$ops = [];
$vmh = file_get_contents(__dir__ . '/vm.h');
if (preg_match_all('/\s+OPC_(\w+)(?=,|\s=)/', $vmh, $mth)) {
    $ops = $mth[1];
}

if (empty($ops)) {
    exit("unable to parse opcodes");
}

$out = fopen(__dir__ . '/vm.inc', 'w+');
foreach ($ops as $op) {
    $u = strtoupper($op);
    $l = strtolower($op);
    fwrite($out, "case OPC_{$u}:\n  rs = op_{$l}(vm, op);\n  break;\n");
}
fclose($out);
