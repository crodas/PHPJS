--TEST--
Simple excpetion tests
--FILE--
<?php 
function global_() {
    throw new Exception("hi there");
}
$js = new JS;
$ret = $js->evaluate(<<<EOF
try {
    \$PHP.global_();
    print("This block of code won't be executed");
} catch (e) {
    print("expected exception: " + e);
}
EOF
);
--EXPECTF--
expected exception: hi there
