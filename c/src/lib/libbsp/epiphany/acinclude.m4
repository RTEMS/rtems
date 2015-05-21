# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  epiphany_sim )
    AC_CONFIG_SUBDIRS([epiphany_sim]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
