# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$RTEMS_BSP_FAMILY" in
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
