# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$RTEMS_BSP_FAMILY" in
  dmv152 )
    AC_CONFIG_SUBDIRS([dmv152]);;
  gen68302 )
    AC_CONFIG_SUBDIRS([gen68302]);;
  gen68340 )
    AC_CONFIG_SUBDIRS([gen68340]);;
  gen68360 )
    AC_CONFIG_SUBDIRS([gen68360]);;
  idp )
    AC_CONFIG_SUBDIRS([idp]);;
  mcf5206elite )
    AC_CONFIG_SUBDIRS([mcf5206elite]);;
  mrm332 )
    AC_CONFIG_SUBDIRS([mrm332]);;
  mvme136 )
    AC_CONFIG_SUBDIRS([mvme136]);;
  mvme147 )
    AC_CONFIG_SUBDIRS([mvme147]);;
  mvme147s )
    AC_CONFIG_SUBDIRS([mvme147s]);;
  mvme162 )
    AC_CONFIG_SUBDIRS([mvme162]);;
  mvme167 )
    AC_CONFIG_SUBDIRS([mvme167]);;
  ods68302 )
    AC_CONFIG_SUBDIRS([ods68302]);;
  sim68000 )
    AC_CONFIG_SUBDIRS([sim68000]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
