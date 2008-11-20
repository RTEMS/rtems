# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  avrtest )
    AC_CONFIG_SUBDIRS([avrtest]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
