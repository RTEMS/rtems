dnl $Id$

AC_DEFUN([RTEMS_ENABLE_HTTPD],
[
AC_ARG_ENABLE([httpd],
AS_HELP_STRING([--enable-httpd],[enable (GoAhead) httpd (DEPRECATED)]),
[case "${enableval}" in 
  yes) enable_httpd=yes ;;
  no) enable_httpd=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-httpd option) ;;
esac],[enable_httpd=no]) 
])
