dnl $Id$

AC_DEFUN([RTEMS_ENABLE_RDBG],
[
AC_BEFORE([$0], [RTEMS_CHECK_RDBG])dnl

AC_ARG_ENABLE(rdbg,
[AS_HELP_STRING(--enable-rdbg,enable remote debugger)],
[case "${enableval}" in
  yes) RTEMS_HAS_RDBG=yes ;;
  no) RTEMS_HAS_RDBG=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-rdbg option) ;;
esac],[RTEMS_HAS_RDBG=no])
AC_SUBST(RTEMS_HAS_RDBG)dnl
])
