# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$RTEMS_BSP_FAMILY" in
  c4xsim )
    AC_CONFIG_SUBDIRS([c4xsim]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
