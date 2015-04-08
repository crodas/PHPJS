--TEST--
Check for extension presence
--FILE--
<?php 
$js = new JS;
$ret = $js->evaluate(<<<EOF
    function cesar() {
        print("hola (" +  (new Date().getTime()) + ")");
    }
    99;
EOF
);
var_dump($ret);
$js->cesar();
?>
--EXPECTF--
float(99)
hola (%d)
