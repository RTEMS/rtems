# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  no_bsp )
    AC_CONFIG_SUBDIRS([no_bsp]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
