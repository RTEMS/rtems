dnl $Id$

AC_DEFUN(RTEMS_ENABLE_INLINES,
[AC_ARG_ENABLE(rtems-inlines,
AC_HELP_STRING([--enable-rtems-inlines],[enable RTEMS inline functions (default:enabled, disable to use macros)]),
[case "${enableval}" in
  yes) enable_rtems_inlines=yes ;;
  no) enable_rtems_inlines=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-rtems-inlines option) ;;
esac],[enable_rtems_inlines=yes])
])
