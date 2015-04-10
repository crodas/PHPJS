--TEST--
Convert not found functions in Javascript as an PHP exception
--FILE--
<?php 
$js = new JS;
$js->foobar();
--EXPECTF--
Fatal error: Uncaught exception 'JSException' with message 'foobar() is not a javascript function' in %s:%d
Stack trace:
#0 %s(%d): JS->__call('foobar', Array)
#1 %s(%d): JS->foobar()
#2 {main}
  thrown in %s on line %d

