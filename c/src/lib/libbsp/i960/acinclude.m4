# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  cvme961 )
    AC_CONFIG_SUBDIRS([cvme961]);;
  i960sim )
    AC_CONFIG_SUBDIRS([i960sim]);;
  rxgen960 )
    AC_CONFIG_SUBDIRS([rxgen960]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
