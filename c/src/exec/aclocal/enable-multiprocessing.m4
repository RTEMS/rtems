dnl $Id$

AC_DEFUN(RTEMS_ENABLE_MULTIPROCESSING,
[
AC_ARG_ENABLE(multiprocessing,
AC_HELP_STRING([--enable-multiprocessing],[enable multiprocessing interface]),
[case "${enableval}" in 
  yes) RTEMS_HAS_MULTIPROCESSING=yes ;;
  no) RTEMS_HAS_MULTIPROCESSING=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-multiprocessing option) ;;
esac],[RTEMS_HAS_MULTIPROCESSING=no])
AC_SUBST(RTEMS_HAS_MULTIPROCESSING)dnl
])
