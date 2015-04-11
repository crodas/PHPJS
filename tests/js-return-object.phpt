--TEST--
Return an object from JS to PHP
--FILE--
<?php 
class EJS extends JS {
    public function __destruct() {
        echo "js object destroyed\n";
    }
}

$js = new EJS;
$js->evaluate(<<<EOF
function Bar()
{
    this.y = 89
}

function Foobar() {
    this.xx = 99
    this.y  = new Bar;
}

function x() {
    return new Foobar;
}
EOF
);
var_dump($y = $js->x());
var_dump($y->xx);
var_dump($y->y);
var_dump($y->y->y);
unset($js); /* release function */
unset($y);
echo "bye\n";
--EXPECTF--
object(JSObjectWrapper)#2 (0) {
}
float(99)
object(JSObjectWrapper)#3 (0) {
}
float(89)
js object destroyed
bye

