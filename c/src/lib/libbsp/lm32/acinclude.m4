# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  lm32_evr )
    AC_CONFIG_SUBDIRS([lm32_evr]);;
  milkymist )
    AC_CONFIG_SUBDIRS([milkymist]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
