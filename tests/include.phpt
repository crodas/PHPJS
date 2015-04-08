--TEST--
Test simple file inclusion
--FILE--
<?php 
function cesar()
{
    print "hola\n";
}
$js = new JS;
var_dump($js->load(__DIR__ . '/foobar.js'));
$js->x(); /* Call x() from javascript */
--EXPECTF--
float(%f)
foobar


