# PHPJS

***This is super experimental***

Run javascript inside PHP, powered by the awesome [Duktape](http://duktape.org) Javascript engine.

Why?
----

1. It's fun!
2. Javascript is becoming mainstream, with hundreds of libraries. Having an easy way of sharing code with Javascript natively makes using
3. 

How to install it?
------------------

```bash
phpize
./configure --enable-js
make install
```

Then add `extension=phpjs.so` to your php.ini

How to use it?

There is `JS` class, each instance runs it's own Javascript *virtual machine* (or duktape's context).

```php
$js1 = new JS;
$js1->load("foobar.js");
```

The `JS` object is like a proxy between the `Javascript` and the `PHP` userlands. In the PHP side, the `JS` looks like an Array or an object.

```php
$js['foobar'] = 1; // Set `foobar` inside Javascript (global variable)
var_dump($js['XXX']); // Read global variable 'XXX' from javascript
$js->fnc(); // Call fnc() function from Javascript.
```

Inside `Javascript` there are also some intergration. For instance we have a `$PHP` (or PHP) global object.

```js
$PHP.var_dump("something"); // Call a PHP function!
$PHP.$something = 1; // set a variable
print($PHP.$something_else); // read a variable from PHP
```

TODO
----

1. Better sharing of objects between PHP and Javascript
2. More documentation
3. More integration :-)
