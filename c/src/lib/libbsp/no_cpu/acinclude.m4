# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$RTEMS_BSP_FAMILY" in
  no_bsp )
    AC_CONFIG_SUBDIRS([no_bsp]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
