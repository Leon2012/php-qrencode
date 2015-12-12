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

#include <gd.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
//#include "ext/gd/libgd/gd.h"
#include "php_qrencode.h"
#include "SAPI.h"
#include <qrencode.h>

//#include "main/php_compat.h"
//#include "ext/gd/php_gd.h"


#define QRENCODE_RESOURCE_TYPE "Qrencode"

//fix buy main/php_compat.h
#define gdImageCreate gdImageCreate
#define gdImagePng gdImagePng
#define gdImageDestroy gdImageDestroy
#define gdImageColorAllocate gdImageColorAllocate
#define gdImageFill gdImageFill
#define gdImageFilledRectangle gdImageFilledRectangle

gdImagePtr qrcode_png(QRcode *code, int fg_color[3], int bg_color[3], int size, int margin);

ZEND_DECLARE_MODULE_GLOBALS(qrencode)

/* True global resources - no need for thread safety here */
static int le_qrencode;
typedef struct _qrencode {
    QRcode *code;
}qrencode;


ZEND_BEGIN_ARG_INFO(qrencode_create_arginfo, 0)
    ZEND_ARG_INFO(0, str)
    ZEND_ARG_INFO(0, level)
    ZEND_ARG_INFO(0, hint)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO(qrencode_save_arginfo, 0)
    ZEND_ARG_INFO(0, qrencode)
    ZEND_ARG_INFO(0, path)

    ZEND_ARG_INFO(0, red)
    ZEND_ARG_INFO(0, green)
    ZEND_ARG_INFO(0, blue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(qrencode_output_arginfo, 0)
    ZEND_ARG_INFO(0, qrencode)

    ZEND_ARG_INFO(0, red)
    ZEND_ARG_INFO(0, green)
    ZEND_ARG_INFO(0, blue)
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
    PHP_FE(qrencode_output,  qrencode_output_arginfo)
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

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("qrencode.version",      "3", PHP_INI_ALL, OnUpdateLong, version, zend_qrencode_globals, qrencode_globals)
    STD_PHP_INI_ENTRY("qrencode.level",      "2", PHP_INI_ALL, OnUpdateLong, level, zend_qrencode_globals, qrencode_globals)
    STD_PHP_INI_ENTRY("qrencode.hint",      "2", PHP_INI_ALL, OnUpdateLong, hint, zend_qrencode_globals, qrencode_globals)
    STD_PHP_INI_ENTRY("qrencode.casesensitive", "1", PHP_INI_ALL, OnUpdateLong, casesensitive, zend_qrencode_globals, qrencode_globals)
PHP_INI_END()


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
	REGISTER_INI_ENTRIES();

    REGISTER_LONG_CONSTANT("QRENCODE_QRECLEVEL_L", QR_ECLEVEL_L, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_QRECLEVEL_M", QR_ECLEVEL_M, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_QRECLEVEL_Q", QR_ECLEVEL_Q, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_QRECLEVEL_H", QR_ECLEVEL_H, CONST_CS|CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("QRENCODE_MODE_NUL", QR_MODE_NUL, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_MODE_NUM", QR_MODE_NUM, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_MODE_AN", QR_MODE_AN, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_MODE_8", QR_MODE_8, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_MODE_KANJI", QR_MODE_KANJI, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_MODE_STRUCTURE", QR_MODE_STRUCTURE, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_MODE_ECI", QR_MODE_ECI, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_MODE_FNC1FIRST", QR_MODE_FNC1FIRST, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QRENCODE_MODE_FNC1SECOND", QR_MODE_FNC1SECOND, CONST_CS|CONST_PERSISTENT);

    le_qrencode = zend_register_list_destructors_ex(qrencode_dtor, NULL, QRENCODE_RESOURCE_TYPE, module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(qrencode)
{

	UNREGISTER_INI_ENTRIES();
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

    long level = 0;
    long hint  = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &str, &str_len, &level, &hint) == FAILURE) {
        RETURN_NULL();
    }

    int version = (int)QRENCODE_G(version);
    int casesensitive = (int)QRENCODE_G(casesensitive);
    QRecLevel q_level = (int)QRENCODE_G(level);
    QRencodeMode q_hint = (int)QRENCODE_G(hint);

    if (level >= 0 && level <= 3) {
        q_level = level;
    }

    if (hint >= QR_MODE_NUL && hint <= QR_MODE_FNC1SECOND) {
        q_hint = hint;
    }
    
    // php_printf("version:%d\n", version);
    // php_printf("casesensitive:%d\n", casesensitive);
    // php_printf("Level:%d\n", q_level);
    // php_printf("hint:%d\n", q_hint);

    QRcode *code = QRcode_encodeString(str, version, q_level, q_hint, casesensitive);
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
    
    int size = 100;
    int margin = 2;

    int red = 0;
    int green = 0;
    int blue = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|llll", &zqe, &path, &path_len, &size, &red, &green, &blue) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "parse parameter error!!!");
        RETURN_FALSE;
    }

    if (red<0 || red>255) {
        red = 0;
    }
    if (green<0 || green>255) {
        green = 0;
    }
    if (blue<0 || blue>255) {
        blue = 0;
    }

    int int_fg_color [3] = {red,green,blue};


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

static void _qrencode_output_putc(struct gdIOCtx *ctx, int c)
{
    /* without the following downcast, the write will fail
     * (i.e., will write a zero byte) for all
     * big endian architectures:
     */
    unsigned char ch = (unsigned char) c;
    TSRMLS_FETCH();
    php_write(&ch, 1 TSRMLS_CC);
}

static int _qrencode_output_putbuf(struct gdIOCtx *ctx, const void* buf, int l)
{
    TSRMLS_FETCH();
    return php_write((void *)buf, l TSRMLS_CC);
}

static void _qrencode_output_ctxfree(struct gdIOCtx *ctx)
{
    if(ctx) {
        efree(ctx);
    }
}

/*输出*/
PHP_FUNCTION(qrencode_output)
{
    zval *zqe;
    gdIOCtx *ctx = NULL;
    qrencode *qe;
    int int_bg_color[3] = {255,255,255} ;
    int size = 100;
    int margin = 2;
    int red = 0;
    int green = 0;
    int blue = 0;
    int q = -1;
    int f = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|llll", &zqe, &size, &red, &green, &blue) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "parse parameter error!!!");
        RETURN_FALSE;
    }

    if (red<0 || red>255) {
        red = 0;
    }
    if (green<0 || green>255) {
        green = 0;
    }
    if (blue<0 || blue>255) {
        blue = 0;
    }

    int int_fg_color [3] = {red,green,blue};
    ZEND_FETCH_RESOURCE(qe, qrencode*, &zqe, -1, QRENCODE_RESOURCE_TYPE, le_qrencode);

    gdImagePtr im = qrcode_png(qe->code,int_fg_color,int_bg_color,size,margin) ;

    //输出Content-type
    char *content_type = "Content-type: image/png";
    sapi_header_line ctr = {0};
    ctr.line = content_type;
    ctr.line_len = strlen(content_type);
    ctr.response_code = 200;
    sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);

    //输出
    ctx = emalloc(sizeof(gdIOCtx));
    ctx->putC = _qrencode_output_putc;
    ctx->putBuf = _qrencode_output_putbuf;
    ctx->gd_free = _qrencode_output_ctxfree;

    gdImagePngCtxEx(im, ctx, -1);
    ctx->gd_free(ctx);


    QRcode_free(qe->code);
    qe->code = NULL;
    gdImageDestroy(im);

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
