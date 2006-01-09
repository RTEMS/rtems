# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  erc32 )
    AC_CONFIG_SUBDIRS([erc32]);;
  leon2 )
    AC_CONFIG_SUBDIRS([leon2]);;
  leon3 )
    AC_CONFIG_SUBDIRS([leon3]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
