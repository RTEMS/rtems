AC_DEFUN([RTEMS_ENABLE_RTEMS_DEBUG],
[
AC_ARG_ENABLE(rtems-debug,
AS_HELP_STRING([--enable-rtems-debug],[enable RTEMS_DEBUG]),
[case "${enable_rtems_debug}" in
  yes) enable_rtems_debug=yes ;;
  no)  enable_rtems_debug=no ;;
  *) AC_MSG_ERROR([bad value ${enable_rtems_debug} for RTEMS_DEBUG]) ;;
esac],[enable_rtems_debug=no])
])
