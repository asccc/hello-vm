#!/usr/bin/env php
<?php

echo implode(' ', array_map(function ($file) {
  return basename($file, '.c') . '.o';
}, glob(__DIR__ . '/*.c')));
