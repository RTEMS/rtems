# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  ep1a )
    AC_CONFIG_SUBDIRS([ep1a]);;
  gen405 )
    AC_CONFIG_SUBDIRS([gen405]);;
  gen5200 )
    AC_CONFIG_SUBDIRS([gen5200]);;
  gen83xx )
    AC_CONFIG_SUBDIRS([gen83xx]);;
  helas403 )
    AC_CONFIG_SUBDIRS([helas403]);;
  mbx8xx )
    AC_CONFIG_SUBDIRS([mbx8xx]);;
  motorola_powerpc )
    AC_CONFIG_SUBDIRS([motorola_powerpc]);;
  mpc8260ads )
    AC_CONFIG_SUBDIRS([mpc8260ads]);;
  mvme3100 )
    AC_CONFIG_SUBDIRS([mvme3100]);;
  mvme5500 )
    AC_CONFIG_SUBDIRS([mvme5500]);;
  psim )
    AC_CONFIG_SUBDIRS([psim]);;
  score603e )
    AC_CONFIG_SUBDIRS([score603e]);;
  ss555 )
    AC_CONFIG_SUBDIRS([ss555]);;
  virtex )
    AC_CONFIG_SUBDIRS([virtex]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
