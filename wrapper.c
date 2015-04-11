#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "php_phpjs.h"

void phpjs_wrapped_free(phpjs_wrap_duk_t * obj TSRMLS_DC)
{
    zend_object_std_dtor(&obj->zo TSRMLS_CC);
    if (obj->vm) {
        zval_ptr_dtor(&obj->vm);
    }
    efree(obj);
}

void phpjs_add_duk_context(zval * this, duk_context * ctx, duk_idx_t idx TSRMLS_DC)
{
    phpjs_wrap_duk_t * obj = (phpjs_wrap_duk_t *) zend_object_store_get_object(this TSRMLS_CC );   
    duk_memory_functions mem;
    duk_get_memory_functions(ctx, &mem);
    obj->vm  = mem.udata;
    obj->ctx = ctx;
    obj->function = duk_require_heapptr(ctx, idx);
    Z_ADDREF_P(obj->vm);
}
