# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  c4xsim )
    AC_CONFIG_SUBDIRS([c4xsim]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
