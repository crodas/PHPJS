--TEST--
Simple excpetion tests
--FILE--
<?php 
$js = new JS;
$ret = $js->evaluate(<<<EOF
kljdalksjda();
EOF
);
--EXPECTF--
Fatal error: Uncaught exception 'JSException' with message 'ReferenceError: identifier 'kljdalksjda' undefined' in %s:%d
Stack trace:
#0 %s(%d): JS->evaluate('kljdalksjda();')
#1 {main}
  thrown in %s on line %d

