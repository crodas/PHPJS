--TEST--
Check for extension presence
--FILE--
<?php 
function cesar()
{
    print "hola\n";
}
$js = new JS;
$ret = $js->evaluate(<<<EOF
    var XY = 1;
    function assert(expr, text) {
        if (!expr) throw new Error(text);
    }

    function cesar() {
        print(JSON.stringify(zarr))
        print(print(zbool, zint));
        PHP.cesar();
        PHP.\$foo = 1;
        PHP.\$obj_js = { cesar : 'rodas', x: [1,2,3, { foo: 'bar'}]}
        assert(PHP.\$fooxx === undefined, "Undefined when a variable doesn't exists")
        assert(PHP.\$foo === 1, "Assert proper type")
        assert(PHP.\$foo !== "1", "Assert proper type")
        try {
            PHP.xesar_xxx();
        } catch (e) {
            print("expected error");
        }
    }
EOF
);

// set some variables 
// on the JS global scope
$js['zbool'] = true;
$js['zint']  = 99;
$js['zarr']  = array(1,2,3,4,5, 'cesar' => 7, array(1,2,3));
assert($js['XY'] == 1);

// check that JS print is using
// PHP's buffer
ob_start();
$js->cesar();
ob_get_clean();
//

// call the func again
$js->cesar();

// read variable that was inject from JS
var_dump($foo);
var_dump($obj_js);

?>
--EXPECT--
{"0":1,"1":2,"2":3,"3":4,"4":5,"cesar":7,"5":[1,2,3]}
true 99
true
hola
expected error
float(1)
array(2) {
  ["cesar"]=>
  string(5) "rodas"
  ["x"]=>
  array(4) {
    [0]=>
    float(1)
    [1]=>
    float(2)
    [2]=>
    float(3)
    [3]=>
    array(1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
}
