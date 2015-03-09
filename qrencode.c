/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_qrencode.h"
#include <qrencode.h>
#include <gd.h>


#define QRENCODE_RESOURCE_TYPE "Qrencode"
gdImagePtr qrcode_png(QRcode *code, int fg_color[3], int bg_color[3], int size, int margin);

/* If you declare any globals in php_qrencode.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(qrencode)
*/

/* True global resources - no need for thread safety here */
static int le_qrencode;
typedef struct _qrencode {
    QRcode *code;
}qrencode;


ZEND_BEGIN_ARG_INFO(qrencode_create_arginfo, 0)
    ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO(qrencode_save_arginfo, 0)
    ZEND_ARG_INFO(0, qrencode)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

/* {{{ qrencode_functions[]
 *
 * Every user visible function must have an entry in qrencode_functions[].
 */
const zend_function_entry qrencode_functions[] = {
	PHP_FE(qrencode_version,	NULL)		/* For testing, remove later. */
    //PHP_FE(gd_version, NULL)
    PHP_FE(qrencode_create, qrencode_create_arginfo)
    PHP_FE(qrencode_save, qrencode_save_arginfo)
	PHP_FE_END	/* Must be the last line in qrencode_functions[] */
};
/* }}} */

/* {{{ qrencode_module_entry
 */
zend_module_entry qrencode_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"qrencode",
	qrencode_functions,
	PHP_MINIT(qrencode),
	PHP_MSHUTDOWN(qrencode),
	PHP_RINIT(qrencode),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(qrencode),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(qrencode),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_QRENCODE_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_QRENCODE
ZEND_GET_MODULE(qrencode)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("qrencode.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_qrencode_globals, qrencode_globals)
    STD_PHP_INI_ENTRY("qrencode.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_qrencode_globals, qrencode_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_qrencode_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_qrencode_init_globals(zend_qrencode_globals *qrencode_globals)
{
	qrencode_globals->global_value = 0;
	qrencode_globals->global_string = NULL;
}
*/
/* }}} */


/*释放resource资源，在脚本结束后执行*/
static void qrencode_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    qrencode *q = (qrencode *)rsrc->ptr;
    if (q->code != NULL) {
        //QRcode_free(q->code);
        q->code = NULL;
    }
    efree(q);
}


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(qrencode)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/

    le_qrencode = zend_register_list_destructors_ex(qrencode_dtor, NULL, QRENCODE_RESOURCE_TYPE, module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(qrencode)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(qrencode)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(qrencode)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(qrencode)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "qrencode support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */



// PHP_FUNCTION(qrcode_version)
// {
//     RETURN_STRING(QRcode_APIVersionString(), 1);
// }

// PHP_FUNCTION(gd_version)
// {
//     RETURN_STRING(GD_VERSION_STRING, 1);
// }

PHP_FUNCTION(qrencode_version)
{
    RETURN_STRING(PHP_QRENCODE_VERSION, 1);
}


/*实例化并注册resource*/
PHP_FUNCTION(qrencode_create)
{
    char *str;
    int str_len;
    qrencode *qe;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
        RETURN_NULL();
    }

    int version = 3;
    QRecLevel level = 2;
    QRencodeMode hint = QR_MODE_8;
    int casesensitive =1;

    
    QRcode *code = QRcode_encodeString(str, version, level, hint, casesensitive);
    if (code == NULL) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "QRcode_encodeString error!!!");
        RETURN_NULL();
    }

    qe = emalloc(sizeof(qrencode));
    qe->code = code;

    ZEND_REGISTER_RESOURCE(return_value, qe, le_qrencode);
}


/*保存*/
PHP_FUNCTION(qrencode_save) 
{
    zval *zqe;
    qrencode *qe;
    char *path;
    int path_len;

    int int_bg_color[3] = {255,255,255} ;
    int int_fg_color [3] = {0,0,0};
    int size = 100;
    int margin = 2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zqe, &path, &path_len) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "parse parameter error!!!");
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(qe, qrencode*, &zqe, -1, QRENCODE_RESOURCE_TYPE, le_qrencode);

    FILE * out = fopen(path,"w+");
    if (out == NULL){
        php_error_docref (NULL TSRMLS_CC, E_WARNING, "can not open the file");
        RETURN_FALSE;
    }

    gdImagePtr im = qrcode_png(qe->code,int_fg_color,int_bg_color,size,margin) ;
    gdImagePng(im,out);
    QRcode_free(qe->code);
    qe->code = NULL;
    gdImageDestroy(im);
    fclose(out);
    RETURN_TRUE;
}


gdImagePtr qrcode_png(QRcode *code, int fg_color[3], int bg_color[3], int size, int margin)
{
    int code_size = size / code->width;
    code_size = (code_size == 0)  ? 1 : code_size;
    int img_width = code->width * code_size + 2 * margin;
    gdImagePtr img = gdImageCreate (img_width,img_width);
    int img_fgcolor =  gdImageColorAllocate(img,fg_color[0],fg_color[1],fg_color[2]);
    int img_bgcolor = gdImageColorAllocate(img,bg_color[0],bg_color[1],bg_color[2]);
    gdImageFill(img,0,0,img_bgcolor);
    unsigned char *p = code->data;
    int x,y ,posx,posy;
    for (y = 0 ; y < code->width ; y++)
    {
        for (x = 0 ; x < code->width ; x++)
        {
            if (*p & 1)
            {
                posx = x * code_size + margin;
                posy = y * code_size + margin;
                gdImageFilledRectangle(img,posx,posy,posx + code_size,posy + code_size,img_fgcolor);
            }
            p++;
        }
    }
    return img;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
