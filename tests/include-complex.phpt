--TEST--
Test simple file inclusion
--FILE--
<?php 
$js = new JS;
$js->load(__DIR__ . '/uglify.js');
$js->load(__DIR__ . '/minify.js');
var_dump($js->u("(function() {
    var someVariable = 99;
    return someVariable;
})"));
--EXPECTF--
string(32) "(function(){var %c=99;return %c});"

