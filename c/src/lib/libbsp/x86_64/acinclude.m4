# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  amd64 )
    AC_CONFIG_SUBDIRS([amd64]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
