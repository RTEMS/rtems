# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$RTEMS_BSP_FAMILY" in
  posix )
    AC_CONFIG_SUBDIRS([posix]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
