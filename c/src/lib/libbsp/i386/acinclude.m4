# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  RTEMS_BSP_ALIAS(ifelse([$1],,[${RTEMS_BSP}],[$1]),bspdir)
  case "$bspdir" in
  i386ex )
    AC_CONFIG_SUBDIRS([i386ex]);;
  pc386 )
    AC_CONFIG_SUBDIRS([pc386]);;
  ts_386ex )
    AC_CONFIG_SUBDIRS([ts_386ex]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
