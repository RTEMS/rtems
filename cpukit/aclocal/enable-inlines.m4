dnl $Id$

AC_DEFUN(RTEMS_ENABLE_INLINES,
[AC_ARG_ENABLE(rtems-inlines,
AC_HELP_STRING([--enable-rtems-inlines],[enable RTEMS inline functions (default:enabled, disable to use macros)]),
[case "${enableval}" in
  yes) RTEMS_USE_MACROS=no ;;
  no) RTEMS_USE_MACROS=yes ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for disable-rtems-inlines option) ;;
esac],[RTEMS_USE_MACROS=no])
AC_SUBST(RTEMS_USE_MACROS)dnl

if test x"${RTEMS_USE_MACROS}" = x"yes";
then
  AC_DEFINE_UNQUOTED(USE_MACROS,1,[if using macros])
else
  AC_DEFINE_UNQUOTED(USE_INLINES,1,[if using inlines])
fi


])
