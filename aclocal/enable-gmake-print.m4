dnl $Id$
dnl
dnl Note: This option is considered obsolete

AC_DEFUN(RTEMS_ENABLE_GMAKE_PRINT,
[
AC_ARG_ENABLE(gmake-print-directory,
[  --enable-gmake-print-directory       enable GNU Make's print directory],
[case "${enableval}" in 
  yes) RTEMS_USE_OWN_PDIR=no ;;
  no) RTEMS_USE_OWN_PDIR=yes ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for gmake-print-directory option)
;;
esac],[RTEMS_USE_OWN_PDIR=yes])
AC_SUBST(RTEMS_USE_OWN_PDIR)dnl
])
