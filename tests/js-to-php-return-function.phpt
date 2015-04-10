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
function x() {
    return function() {
        return function() {
            return function() {
                print("Hi!");
                return parseInt(Math.random()*0xfffff);
            };
        }
    };
}
EOF
);
var_dump($y = $js->x());
unset($js); /* release function */
var_dump($x = $y());
var_dump($z = $x());
var_dump($z());
unset($y, $x, $z); /* destroy object proxies */
echo "bye\n";
--EXPECTF--
object(JSFunctionWrapper)#2 (0) {
}
object(JSFunctionWrapper)#3 (0) {
}
object(JSFunctionWrapper)#4 (0) {
}
Hi!
float(%d)
js object destroyed
bye
