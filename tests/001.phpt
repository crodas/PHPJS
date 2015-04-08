--TEST--
Check for extension presence
--SKIPIF--
<?php if (!extension_loaded("phpjs")) print "skip"; ?>
--FILE--
<?php 
echo "phpjs extension is available";
?>
--EXPECT--
phpjs extension is available
