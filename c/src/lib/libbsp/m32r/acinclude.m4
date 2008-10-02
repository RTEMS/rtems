# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  m32rsim )
    AC_CONFIG_SUBDIRS([m32rsim]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
