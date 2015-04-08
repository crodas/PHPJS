--TEST--
Check for extension presence
--SKIPIF--
<?php if (!extension_loaded("phpjs")) print "skip"; ?>
--FILE--
<?php 
var_dump(class_exists('JS'));
var_dump(class_exists('JSFunctionWrapper'));
var_dump(class_exists('JSException'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
