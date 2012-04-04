# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  csb350 )
    AC_CONFIG_SUBDIRS([csb350]);;
  genmongoosev )
    AC_CONFIG_SUBDIRS([genmongoosev]);;
  hurricane )
    AC_CONFIG_SUBDIRS([hurricane]);;
  jmr3904 )
    AC_CONFIG_SUBDIRS([jmr3904]);;
  malta )
    AC_CONFIG_SUBDIRS([malta]);;
  rbtx4925 )
    AC_CONFIG_SUBDIRS([rbtx4925]);;
  rbtx4938 )
    AC_CONFIG_SUBDIRS([rbtx4938]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
