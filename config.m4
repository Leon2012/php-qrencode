PHP_ARG_WITH(qrencode, for qrencode support,
 [  --with-qrencode             Include qrencode support])


PHP_ARG_ENABLE(qrencode, whether to enable qrencode support,
 [  --enable-qrencode           Enable qrencode support])

if test "$PHP_QRENCODE" != "no"; then
   SEARCH_PATH="/usr/local /usr"     
   SEARCH_FOR="/include/qrencode.h"  
   if test -r $PHP_QRENCODE/$SEARCH_FOR; then # path given as parameter
     QRENCODE_DIR=$PHP_QRENCODE
   else 
     AC_MSG_CHECKING([for qrencode files in default path])
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         QRENCODE_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
   fi
  
   if test -z "$QRENCODE_DIR"; then
     AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Please reinstall the qrencode distribution])
   fi


   PHP_ADD_INCLUDE($QRENCODE_DIR/include)


   LIBNAME=qrencode 
   LIBSYMBOL=QRcode_APIVersionString  

   PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
   [
     PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $QRENCODE_DIR/$PHP_LIBDIR, QRENCODE_SHARED_LIBADD)
     PHP_ADD_LIBRARY_WITH_PATH(gd, $QRENCODE_DIR/$PHP_LIBDIR, QRENCODE_SHARED_LIBADD)

     AC_DEFINE(HAVE_QRENCODELIB,1,[ ])
   ],[
     AC_MSG_ERROR([wrong qrencode lib version or lib not found])
   ],[
     -L$QRENCODE_DIR/$PHP_LIBDIR -lm
   ])
  
   PHP_SUBST(QRENCODE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(qrencode, qrencode.c, $ext_shared)
fi


dnl PHP_ARG_WITH(gd, for libgd support, 
dnl   [ --with-gd[=DIR]             Include gd support])

dnl if test "$PHP_GD" != "no"; then
dnl   if test -r $PHP_GD/lib/libgd.a; then
dnl     GD_DIR=$PHP_GD
dnl   else
dnl     AC_MSG_CHECKING(for libgd in default path)
dnl     for i in /usr/local/opt/gd /usr/local /usr ; do
dnl       if test -r $i/lib/libgd.a; then
dnl         GD_DIR=$i
dnl         AC_MSG_RESULT(found in $i)
dnl       fi
dnl     done
dnl   fi
dnl   
dnl   if test -z "$GD_DIR"; then
dnl     AC_MSG_RESULT(not found)
dnl    AC_MSG_ERROR(please install gd )
dnl   fi
dnl   PHP_ADD_INCLUDE($GD_DIR/include)

dnl   PHP_ADD_LIBRARY_WITH_PATH(gd, $GD_DIR/lib, QRENCODE_SHARED_LIBADD)
dnl   AC_DEFINE(HAVE_GD,1,[ ])
dnl   
dnl   PHP_SUBST(QRENCODE_SHARED_LIBADD)
dnl 
dnl   PHP_EXTENSION(qrencode, $ext_shared)
dnl fi


if test -z "$PHP_DEBUG"; then 
  AC_ARG_ENABLE(debug,
  [ --enable-debug      compile with debugging symbols],[
    PHP_DEBUG=$enableval
  ],[ PHP_DEBUG=no
  ])
fi
