# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  gdbv850sim )
    AC_CONFIG_SUBDIRS([gdbv850sim]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
