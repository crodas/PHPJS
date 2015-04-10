#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "phpjs.h"

PHPAPI zend_class_entry *phpjs_JSFunctionWrapper_ptr;

typedef struct {
    zend_object zo;
    duk_context * ctx;
    zval * vm;
    void * function;
} phpjs_function;

static void phpjs_function_free(phpjs_function * obj TSRMLS_DC)
{
    zend_object_std_dtor(&obj->zo TSRMLS_CC);
    if (obj->vm) {
        zval_ptr_dtor(&obj->vm);
    }
    efree(obj);
}

static zend_object_value phpjs_function_new(zend_class_entry * ce TSRMLS_DC)
{
    zend_object_value retval;
    phpjs_function * obj;

    obj = (phpjs_function *) emalloc(sizeof(phpjs_function));
    memset(obj, 0, sizeof(phpjs_function));
    zend_object_std_init(&obj->zo, ce TSRMLS_CC);

    object_properties_init((zend_object*) &(obj->zo), ce);

    retval.handle = zend_objects_store_put(obj, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)phpjs_function_free, NULL TSRMLS_CC);
    retval.handlers = zend_get_std_object_handlers();

    return retval;
}

void phpjs_JSFunctionWrapper_setContext(zval * this, duk_context * ctx, duk_idx_t idx)
{
    TSRMLS_FETCH();
    phpjs_function * obj = (phpjs_function *) zend_object_store_get_object(this TSRMLS_CC );   
    duk_memory_functions mem;
    duk_get_memory_functions(ctx, &mem);
    obj->vm  = mem.udata;
    obj->ctx = ctx;
    obj->function = duk_require_heapptr(ctx, idx);
    Z_ADDREF_P(obj->vm);

}

ZEND_METHOD(JSFunctionWrapper, __invoke)
{
    int argc = 0;
    phpjs_function * obj = (phpjs_function *) zend_object_store_get_object(getThis() TSRMLS_CC );   
    if (!this_ptr || !instanceof_function(Z_OBJCE_P(this_ptr), phpjs_JSFunctionWrapper_ptr TSRMLS_CC) || obj->ctx == NULL) {   
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unexpected error. This method cannot be called statically");
        return;
    } 

    duk_push_heapptr(obj->ctx, obj->function);

    if (duk_pcall(obj->ctx, argc) != 0) {
        duk_php_throw(obj->ctx, -1 TSRMLS_CC);
        RETURN_FALSE;
    }
    duk_to_zval(&return_value, obj->ctx, -1);
    duk_pop(obj->ctx); /* bye return */
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
    phpjs_JSFunctionWrapper_ptr->create_object = phpjs_function_new;
}
