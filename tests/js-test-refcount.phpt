--TEST--
Return a function from JS to PHP
--FILE--
<?php 
class EJS extends JS {
    public function __destruct() {
        echo "js object destroyed\n";
    }
}
$js = new EJS;
$js->evaluate(<<<EOF
var x = (new function Foobar() {
    var i = Math.random();
    this.x = function() {
        return i;
    }
});

function y() {
    return x;
}
EOF
);
var_dump($y1 = $js->y());
var_dump($y2 = $js->y());
var_dump($y3 = $js->y());
var_dump($y1->x() === $y2->x());
unset($js); /* release function */
var_dump($y3->x() === $y2->x());
var_dump($method = $y1->x);
var_dump($method() == $y3->x());
unset($method);
unset($y1);
unset($y2);
unset($y3);
echo "bye\n";
--EXPECTF--
object(JSObjectWrapper)#2 (0) {
}
object(JSObjectWrapper)#3 (0) {
}
object(JSObjectWrapper)#4 (0) {
}
bool(true)
bool(true)
object(JSFunctionWrapper)#5 (0) {
}
bool(true)
js object destroyed
bye
