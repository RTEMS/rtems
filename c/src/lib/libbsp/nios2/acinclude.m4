# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  nios2_iss )
    AC_CONFIG_SUBDIRS([nios2_iss]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
