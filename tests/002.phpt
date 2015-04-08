--TEST--
Check for extension presence
--FILE--
<?php 
var_dump(class_exists("js"));
$js = new JS;
?>
--EXPECT--
bool(true)
