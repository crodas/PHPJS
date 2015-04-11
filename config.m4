PHP_ARG_ENABLE(phpjs, [whether to enable phpjs support],
[  --enable-phpjs          Enable phpjs support])

if test "$PHP_PHPJS" = "yes"; then
  PHP_NEW_EXTENSION(phpjs, phpjs.c duktape.c interface.c functions.c objects.c wrapper.c,$ext_shared,,$P2C_CFLAGS)
fi
