# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  orp )
    AC_CONFIG_SUBDIRS([orp]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
