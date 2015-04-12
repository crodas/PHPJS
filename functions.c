#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "php_phpjs.h"

PHPAPI zend_class_entry *phpjs_JSFunctionWrapper_ptr;

static zend_object_value phpjs_function_new(zend_class_entry * ce TSRMLS_DC)
{
    zend_object_value retval;
    phpjs_wrap_duk_t * obj;

    obj = (phpjs_wrap_duk_t *) emalloc(sizeof(phpjs_wrap_duk_t));
    memset(obj, 0, sizeof(phpjs_wrap_duk_t));
    zend_object_std_init(&obj->zo, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
    zval * tmp;
    zend_hash_copy(obj->zo.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
#else
    object_properties_init((zend_object*) &(obj->zo), ce);
#endif

    retval.handle = zend_objects_store_put(obj, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)phpjs_wrapped_free, NULL TSRMLS_CC);
    retval.handlers = zend_get_std_object_handlers();

    return retval;
}

ZEND_METHOD(JSFunctionWrapper, __invoke)
{
    int argc = 0;
    FETCH_THIS_WRAPPER

    duk_dup(ctx, obj->idx);
    if (duk_pcall(ctx, argc) != 0) {
        duk_php_throw(ctx, -1 TSRMLS_CC);
        RETURN_FALSE;
    }
    duk_to_zval(&return_value, obj->ctx, -1);
    php_duk_free_return(ctx);

}

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JSFunctionWrapper___invoke, 0, 0, 0)
ZEND_END_ARG_INFO()

static zend_function_entry phpjs_JSFunctionWrapper_functions[] = {
    ZEND_ME(JSFunctionWrapper, __invoke, ai_phpjs_JSFunctionWrapper___invoke, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

void php_register_function_handler(TSRMLS_D)
{
    zend_class_entry _ce;
    INIT_CLASS_ENTRY(_ce, "JSFunctionWrapper", phpjs_JSFunctionWrapper_functions);
    phpjs_JSFunctionWrapper_ptr = zend_register_internal_class(&_ce TSRMLS_CC);
    phpjs_JSFunctionWrapper_ptr->create_object = phpjs_function_new;
}
