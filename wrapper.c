#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "php_phpjs.h"

int phpjs_php__call(duk_context * ctx, char * fnc, zval * a_args, zval * return_value TSRMLS_DC)
{
    duk_get_prop_string(ctx, -1, fnc);

    if (!duk_is_function(ctx, -1)) {
        duk_pop(ctx);
        char * exception = emalloc(1024 + strlen(fnc));
        sprintf(exception, "%s() is not a javascript function", fnc);
        THROW_EXCEPTION(exception);
        efree(exception);
        RETURN_FALSE;
    }

    int argc = zval_array_to_stack(ctx, a_args);

    if (duk_pcall(ctx, argc) != 0) {
        duk_php_throw(ctx, -1 TSRMLS_CC);
        RETURN_FALSE;
    }
    duk_to_zval(&return_value, ctx, -1);
    php_duk_free_return(ctx);
}

int zval_array_to_stack(duk_context * ctx, zval * a_args)
{
    int argc = 0;
    zval ** data;
    HashTable *myht = Z_ARRVAL_P(a_args);
    
    for (zend_hash_internal_pointer_reset(myht);
            zend_hash_get_current_data(myht, (void **) &data) == SUCCESS;
            zend_hash_move_forward(myht)
        ) {
        zval_to_duk(ctx, NULL, *data);
        argc++;
    }

    return argc;
}

void phpjs_wrapped_free(phpjs_wrap_duk_t * obj TSRMLS_DC)
{
    zend_object_std_dtor(&obj->zo TSRMLS_CC);
    if (obj->vm) {
        duk_to_undefined(obj->ctx, obj->idx);
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
    obj->idx = duk_require_normalize_index(ctx, idx);
    duk_dup(ctx, idx);
    Z_ADDREF_P(obj->vm);
}
