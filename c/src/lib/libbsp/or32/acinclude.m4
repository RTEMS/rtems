# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$RTEMS_BSP_FAMILY" in
  orp )
    AC_CONFIG_SUBDIRS([orp]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
