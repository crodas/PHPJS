#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "phpjs.h"

PHPAPI zend_class_entry *phpjs_JS_ptr;

typedef struct {
    zend_object zo;
    duk_context * ctx;
} php_js_t;


ZEND_METHOD(JS, evaluate)
{
    FETCH_THIS;

    char * str;
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &len) == FAILURE)
        return;

    if (duk_peval_lstring(ctx, str, len) != 0) {
        php_printf("Error: %s\n", duk_safe_to_string(ctx, -1));
        RETURN_FALSE;
    }

    duk_to_zval(&return_value, ctx, -1);
    duk_pop(ctx);
}

// public function load($path)
ZEND_METHOD(JS, load)
{
    FETCH_THIS;
    char * varname;
    int varname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE) {
        return;
    }

    if (duk_peval_file(ctx, varname) != 0) {
        php_printf("Error: %s\n", duk_safe_to_string(ctx, -1));
        RETURN_FALSE;
    }

    duk_to_zval(&return_value, ctx, -1);
    duk_pop(ctx);
}

ZEND_METHOD(JS, offsetUnset)
{
}

ZEND_METHOD(JS, offsetExists)
{
    FETCH_THIS;
    char * varname;
    int varname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE)
        return;

    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, varname);
    duk_to_zval(&return_value, ctx, -1);
}

ZEND_METHOD(JS, offsetGet)
{
    FETCH_THIS;
    char * varname;
    int varname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE)
        return;

    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, varname);
    duk_to_zval(&return_value, ctx, -1);
}


// public function __get($name)
ZEND_METHOD(JS, __get)
{
    FETCH_THIS;
    char * varname;
    int varname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE)
        return;

    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, varname);
    duk_to_zval(&return_value, ctx, -1);
}

duk_ret_t duk_php_print(duk_context * ctx)
{
    int args = duk_get_top(ctx);
    int i;

    for (i = 0; i < args; i++) {
        php_printf(i == args - 1 ? "%s\n" : "%s ", duk_safe_to_string( ctx, i ));
    }

    duk_push_true(ctx);

    return 1;
}

// public function __set($name, $value)
ZEND_METHOD(JS, __set)
{
    FETCH_THIS;
    zval* a_value;
    char * name;
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len, &a_value) == FAILURE)
        return;

    zval_to_duk(ctx, name, a_value);
}

ZEND_METHOD(JS, offsetSet)
{
    FETCH_THIS;
    zval* a_value;
    char * name;
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len, &a_value) == FAILURE)
        return;

    zval_to_duk(ctx, name, a_value);
}


// public function __call($name, $args)
ZEND_METHOD(JS, __call)
{
    FETCH_THIS;
    zval* a_args;
    char * fnc;
    int lfnc;
    int ind;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &fnc, &lfnc, &a_args) == FAILURE)
        return;

    duk_push_global_object(obj->ctx);
    duk_get_prop_string(obj->ctx, -1, fnc);

    if (!duk_is_function(obj->ctx, -1)) {
        duk_pop(obj->ctx);
        zend_error(E_ERROR, "%s() doesnt exists", fnc);
        return;
    }

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


    if (duk_pcall(obj->ctx, argc) != 0) {
        php_printf("Error: %s\n", duk_safe_to_string(obj->ctx, -1));
        RETURN_FALSE;
    }
    duk_to_zval(&return_value, ctx, -1);
    duk_pop(obj->ctx);
}

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_evaluate, 0, 0, 1)
    ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_offsetExists, 0, 0, 1)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_offsetGet, 0, 0, 1)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_offsetSet, 0, 0, 2)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_offsetUnset, 0, 0, 1)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_load, 0, 0, 1)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_export, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS___get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS___set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS___call, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

    static zend_function_entry phpjs_JS_functions[] = {
        ZEND_ME(JS, evaluate, ai_phpjs_JS_evaluate, ZEND_ACC_PUBLIC)
            ZEND_ME(JS, load, ai_phpjs_JS_load, ZEND_ACC_PUBLIC)
            ZEND_ME(JS, offsetExists, ai_phpjs_JS_offsetExists, ZEND_ACC_PUBLIC)
            ZEND_ME(JS, offsetGet, ai_phpjs_JS_offsetGet, ZEND_ACC_PUBLIC)
            ZEND_ME(JS, offsetSet, ai_phpjs_JS_offsetSet, ZEND_ACC_PUBLIC)
            ZEND_ME(JS, offsetUnset, ai_phpjs_JS_offsetUnset, ZEND_ACC_PUBLIC)
            ZEND_ME(JS, __get, ai_phpjs_JS___get, ZEND_ACC_PUBLIC)
            ZEND_ME(JS, __set, ai_phpjs_JS___set, ZEND_ACC_PUBLIC)
            ZEND_ME(JS, __call, ai_phpjs_JS___call, ZEND_ACC_PUBLIC)
        ZEND_FE_END
    };

static zend_function_entry phpjs_functions[] = {
    {NULL, NULL, NULL}
};


static void php_js_destroy(php_js_t *obj TSRMLS_DC)
{
    if (obj->ctx) {
        duk_destroy_heap(obj->ctx);
    }
}

static void php_js_free_storage(php_js_t *obj TSRMLS_DC)
{
    zend_object_std_dtor(&obj->zo TSRMLS_CC);
    php_js_destroy(obj TSRMLS_CC);
    efree(obj);
}


zend_object_value phpjs_new_vm(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    php_js_t * obj;
    zval *tmp;

    obj = (php_js_t *) emalloc(sizeof(php_js_t));
    memset(obj, 0, sizeof(php_js_t));
    zend_object_std_init(&obj->zo, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->zo.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
#else
    object_properties_init((zend_object*) &(obj->zo), ce);
#endif


    obj->ctx = duk_create_heap_default();

    duk_push_global_object(obj->ctx);
    duk_push_c_function(obj->ctx, duk_php_print, DUK_VARARGS);
    duk_put_prop_string(obj->ctx, -2, "print");
    duk_pop(obj->ctx);

    duk_push_global_object(obj->ctx);
    duk_push_c_function(obj->ctx, duk_set_into_php, DUK_VARARGS);
    duk_put_prop_string(obj->ctx, -2, "__set_into_php");
    duk_pop(obj->ctx);

    duk_push_global_object(obj->ctx);
    duk_push_c_function(obj->ctx, duk_get_from_php, DUK_VARARGS);
    duk_put_prop_string(obj->ctx, -2, "__get_from_php");
    duk_pop(obj->ctx);

    duk_push_string(obj->ctx, "var $PHP = new Proxy({}, {       " \
        "set: __set_into_php,                                   " \
        "get: __get_from_php,                                   " \
        "});                                                    " \
        "var PHP = $PHP;                                        " 
    );

    if (duk_peval(obj->ctx) != 0) {
        printf("eval failed: %s\n", duk_safe_to_string(obj->ctx, -1));
    }
    duk_pop(obj->ctx);

    retval.handle = zend_objects_store_put(obj, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)php_js_free_storage, NULL TSRMLS_CC);
    retval.handlers = zend_get_std_object_handlers();

    return retval;
}


PHP_MINIT_FUNCTION(phpjs)
{
    zend_class_entry _ce, *_if;
    zval* _val;

    INIT_CLASS_ENTRY(_ce, "JS", phpjs_JS_functions);
    phpjs_JS_ptr = zend_register_internal_class(&_ce TSRMLS_CC);
    phpjs_JS_ptr->create_object = phpjs_new_vm;
    zend_do_implement_interface(phpjs_JS_ptr, zend_ce_arrayaccess TSRMLS_CC);

    return SUCCESS;
}


PHP_MINFO_FUNCTION(phpjs)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "phpjs", "enabled");
    php_info_print_table_row(2, "Version", "1.0");
    php_info_print_table_end();
}

zend_module_entry phpjs_module_entry = {
    STANDARD_MODULE_HEADER,
    "phpjs",
    phpjs_functions,
    PHP_MINIT(phpjs),
    NULL,
    NULL,
    NULL,
    PHP_MINFO(phpjs),
    "1.0",
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHPJS
ZEND_GET_MODULE(phpjs)
#endif
