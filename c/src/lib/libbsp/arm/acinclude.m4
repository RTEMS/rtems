# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  RTEMS_BSP_ALIAS(ifelse([$1],,[${RTEMS_BSP}],[$1]),bspdir)
  case "$bspdir" in
  arm_bare_bsp )
    AC_CONFIG_SUBDIRS([arm_bare_bsp]);;
  armulator )
    AC_CONFIG_SUBDIRS([armulator]);;
  vegaplus )
    AC_CONFIG_SUBDIRS([vegaplus]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
