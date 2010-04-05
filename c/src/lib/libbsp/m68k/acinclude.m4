# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  av5282 )
    AC_CONFIG_SUBDIRS([av5282]);;
  csb360 )
    AC_CONFIG_SUBDIRS([csb360]);;
  gen68302 )
    AC_CONFIG_SUBDIRS([gen68302]);;
  gen68340 )
    AC_CONFIG_SUBDIRS([gen68340]);;
  gen68360 )
    AC_CONFIG_SUBDIRS([gen68360]);;
  genmcf548x )
    AC_CONFIG_SUBDIRS([genmcf548x]);;
  idp )
    AC_CONFIG_SUBDIRS([idp]);;
  mcf5206elite )
    AC_CONFIG_SUBDIRS([mcf5206elite]);;
  mcf52235 )
    AC_CONFIG_SUBDIRS([mcf52235]);;
  mcf5225x )
    AC_CONFIG_SUBDIRS([mcf5225x]);;
  mcf5235 )
    AC_CONFIG_SUBDIRS([mcf5235]);;
  mcf5329 )
    AC_CONFIG_SUBDIRS([mcf5329]);;
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
  uC5282 )
    AC_CONFIG_SUBDIRS([uC5282]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
