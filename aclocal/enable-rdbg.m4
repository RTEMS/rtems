dnl $Id$

AC_DEFUN(RTEMS_ENABLE_RDBG,
[
AC_ARG_ENABLE(rdbg,
[  --enable-rdbg                        enable remote debugger],
[case "${enableval}" in
  yes) RTEMS_HAS_RDBG=yes ;;
  no) RTEMS_HAS_RDBG=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-rdbg option) ;;
esac],[RTEMS_HAS_RDBG=no])
])
