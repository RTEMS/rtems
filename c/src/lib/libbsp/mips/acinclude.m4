# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  csb350 )
    AC_CONFIG_SUBDIRS([csb350]);;
  genmongoosev )
    AC_CONFIG_SUBDIRS([genmongoosev]);;
  jmr3904 )
    AC_CONFIG_SUBDIRS([jmr3904]);;
  p4000 )
    AC_CONFIG_SUBDIRS([p4000]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
