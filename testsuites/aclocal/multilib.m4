dnl This provides configure definitions used for multilib support

dnl parts of these macros are derived from newlib-1.8.2's multilib support

AC_DEFUN([RTEMS_ENABLE_MULTILIB],
[
AC_ARG_ENABLE(multilib,
AS_HELP_STRING(--enable-multilib,build many library versions (default=no)),
[case "${enableval}" in
  yes) multilib=yes ;;
  no)  multilib=no ;;
  *)   AC_MSG_ERROR(bad value ${enableval} for multilib option) ;;
 esac], [multilib=no])dnl

AM_CONDITIONAL(MULTILIB,test x"${multilib}" = x"yes")
])
