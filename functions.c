#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "phpjs.h"

PHPAPI zend_class_entry *phpjs_JSFunctionWrapper_ptr;

ZEND_METHOD(JSFunctionWrapper, __invoke)
{
}

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JSFunctionWrapper___invoke, 0, 0, 0)
ZEND_END_ARG_INFO()

static zend_function_entry phpjs_JSFunctionWrapper_functions[] = {
    ZEND_ME(JSFunctionWrapper, __invoke, ai_phpjs_JSFunctionWrapper___invoke, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

void php_register_function_handler()
{
    TSRMLS_FETCH();
    zend_class_entry _ce;
    INIT_CLASS_ENTRY(_ce, "JSFunctionWrapper", phpjs_JSFunctionWrapper_functions);
    phpjs_JSFunctionWrapper_ptr = zend_register_internal_class(&_ce TSRMLS_CC);
}
