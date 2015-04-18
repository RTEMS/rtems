# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  generic_or1k )
    AC_CONFIG_SUBDIRS([generic_or1k]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
