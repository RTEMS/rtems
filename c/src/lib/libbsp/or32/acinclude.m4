# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  RTEMS_BSP_ALIAS(ifelse([$1],,[${RTEMS_BSP}],[$1]),bspdir)
  case "$bspdir" in
  or1ksim )
    AC_CONFIG_SUBDIRS([or1ksim]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
