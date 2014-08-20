# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  or1ksim )
    AC_CONFIG_SUBDIRS([or1ksim]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
