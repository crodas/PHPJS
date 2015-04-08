#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "phpjs.h"

void duk_php_throw(duk_context * ctx, duk_idx_t idx)
{
    zval * tc_ex;
    MAKE_STD_ZVAL(tc_ex);
    object_init_ex(tc_ex, phpjs_JSException_ptr);

    char * message = duk_safe_to_string(ctx, idx);

    zend_update_property_string(
        phpjs_JSException_ptr,
        tc_ex,
        "message", 
        sizeof("message") - 1,
        message TSRMLS_CC
    );
    zend_throw_exception_object(tc_ex TSRMLS_CC);
}

void zval_to_duk(duk_context * ctx, char * name, zval * value)
{
    switch (Z_TYPE_P(value)) {
    case IS_ARRAY: {
        zval ** data;
        HashTable *myht = Z_ARRVAL_P(value);
        char *str_index;
        uint str_length;
        ulong num_index;
        duk_idx_t arr_idx = duk_push_php_array_or_object(ctx, myht);


        for (zend_hash_internal_pointer_reset(myht);
               zend_hash_get_current_data(myht, (void **) &data) == SUCCESS;
               zend_hash_move_forward(myht)
           ) {
            zval_to_duk(ctx, NULL, *data);
            switch (zend_hash_get_current_key_ex(myht, &str_index, &str_length, &num_index, 0, NULL)) {
                case HASH_KEY_IS_LONG:
                    duk_put_prop_index(ctx, arr_idx, num_index);
                    break;
                case HASH_KEY_IS_STRING:
                    duk_put_prop_string(ctx, arr_idx, str_index);
                    break;
            }

        }
        break;
    }
    case IS_STRING:
        duk_push_string(ctx, Z_STRVAL_P(value));
        break;
    case IS_LONG:
        duk_push_int(ctx, Z_LVAL_P(value));
        break;
    case IS_DOUBLE:
        duk_push_number(ctx, Z_DVAL_P(value));
        break;
    case IS_BOOL:
        if (Z_BVAL_P(value)) {
            duk_push_true(ctx);
        } else {
            duk_push_false(ctx);
        }
        break;
    case IS_NULL:
        duk_push_false(ctx);
        break;
    }

    if (name) {
        // if there is a name, then create a variable
        // otherwise just put the value to the stack (most likely to return;)
        duk_put_global_string(ctx, name);
    }
}

duk_idx_t duk_push_php_array_or_object(duk_context * ctx, HashTable * myht)
{
    char *str_index;
    uint str_length;
    ulong num_index;
    zval ** data;

    for (zend_hash_internal_pointer_reset(myht);
            zend_hash_get_current_data(myht, (void **) &data) == SUCCESS;
            zend_hash_move_forward(myht)
        ) {
        switch (zend_hash_get_current_key_ex(myht, &str_index, &str_length, &num_index, 0, NULL)) {
        case HASH_KEY_IS_STRING:
                return duk_push_object(ctx);
        }
    }

    return duk_push_array(ctx);
}

void duk_to_zval(zval ** var, duk_context * ctx, duk_idx_t idx)
{
    duk_size_t len;
    char * str;

    switch (duk_get_type(ctx, idx)) {
    case DUK_TYPE_UNDEFINED:
    case DUK_TYPE_NULL:
    case DUK_TYPE_NONE:
        ZVAL_NULL(*var);
        break;

    case DUK_TYPE_OBJECT: {
        duk_idx_t idx1;
        duk_enum(ctx, idx, DUK_ENUM_OWN_PROPERTIES_ONLY);
        idx1 = duk_normalize_index(ctx, -1);
        array_init(*var);

        while (duk_next(ctx, idx1, 1 /*get_value*/)) {
            zval * value;
            MAKE_STD_ZVAL(value);
            duk_to_zval(&value, ctx, -1);

            if(duk_get_type(ctx, -2) == DUK_TYPE_NUMBER) {
                add_index_zval(*var, duk_get_number(ctx,-2), value);
            } else {
                add_assoc_zval(*var, duk_get_string(ctx, -2), value);
            }

            duk_pop_2(ctx);  /* pop_key */
        }
        duk_pop(ctx);
        break;
    }

    case DUK_TYPE_NUMBER:
        ZVAL_DOUBLE(*var, duk_get_number(ctx, idx));
        break;

    case DUK_TYPE_BOOLEAN:
        if (duk_get_number(ctx, idx)) {
            ZVAL_TRUE(*var);
        } else {
            ZVAL_FALSE(*var);
        }
        break;

    case DUK_TYPE_STRING:
        str = duk_get_lstring(ctx, idx, &len);
        ZVAL_STRINGL(*var, str, len,  1);
        break;
    }
}

duk_ret_t php_get_function_wrapper(duk_context * ctx)
{
    char * fnc = "";
    zval *retval, *func;
    int args = duk_get_top(ctx); /* function args */

    MAKE_STD_ZVAL(func);
    MAKE_STD_ZVAL(retval);


    /* get function name (it's a __function property) */
    duk_push_current_function(ctx);
    duk_get_prop_string(ctx, -1, "__function");
    duk_to_zval(&func, ctx, -1);
    duk_pop(ctx);
    /* we got already the function name */

    TSRMLS_FETCH();
    if(call_user_function(CG(function_table), NULL, func, retval, 0, NULL TSRMLS_CC) != SUCCESS) {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "Unknown PHP function: \"%s\"", Z_STRVAL_P(func));
        duk_throw(ctx);
    }

    zval_ptr_dtor(&func);
    zval_ptr_dtor(&retval);

    return 1;
}

duk_ret_t duk_set_into_php(duk_context * ctx)
{
    zval * value;
    char * name;

    MAKE_STD_ZVAL(value);
    duk_to_zval(&value, ctx, 2);

    name = estrdup(duk_get_string(ctx, 1) + 1);

    TSRMLS_FETCH();
    zend_hash_add(EG(active_symbol_table), name, strlen(name)+1, &value, sizeof(zval*), NULL);

    duk_push_true(ctx);

    return 1;
}


duk_ret_t duk_get_from_php(duk_context * ctx)
{
    int args = duk_get_top(ctx);
    char * name = duk_get_string(ctx, 1);

    if (name[0] == '$') {
        zval ** value;
        TSRMLS_FETCH();
        if(zend_hash_find(EG(active_symbol_table), name+1, strlen(name), (void **) &value) == SUCCESS) {
            zval_to_duk(ctx, NULL, *value);
        } else {
            duk_push_undefined(ctx);
        }
    } else {
        // they expect a function wrapper
        duk_push_c_function(ctx, php_get_function_wrapper, DUK_VARARGS);
        duk_push_string(ctx, name);
        duk_put_prop_string(ctx, -2, "__function");
    }


    return 1;
}

