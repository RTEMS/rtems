# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  h8sim )
    AC_CONFIG_SUBDIRS([h8sim]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
