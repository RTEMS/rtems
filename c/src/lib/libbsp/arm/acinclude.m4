# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  arm_bare_bsp )
    AC_CONFIG_SUBDIRS([arm_bare_bsp]);;
  armulator )
    AC_CONFIG_SUBDIRS([armulator]);;
  edb7312 )
    AC_CONFIG_SUBDIRS([edb7312]);;
  vegaplus )
    AC_CONFIG_SUBDIRS([vegaplus]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
