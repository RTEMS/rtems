dnl $Id$

AC_DEFUN(RTEMS_ENABLE_GCC28,
[
AC_ARG_ENABLE(gcc28,
AC_HELP_STRING([--enable-gcc28],[enable use of gcc 2.8.x features]),
[case "${enableval}" in
  yes) RTEMS_USE_GCC272=no ;;
  no) RTEMS_USE_GCC272=yes ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for gcc-28 option) ;;
esac],[RTEMS_USE_GCC272=no])
])
