# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  dmv177 )
    AC_CONFIG_SUBDIRS([dmv177]);;
  eth_comm )
    AC_CONFIG_SUBDIRS([eth_comm]);;
  gen405 )
    AC_CONFIG_SUBDIRS([gen405]);;
  helas403 )
    AC_CONFIG_SUBDIRS([helas403]);;
  mbx8xx )
    AC_CONFIG_SUBDIRS([mbx8xx]);;
  motorola_powerpc )
    AC_CONFIG_SUBDIRS([motorola_powerpc]);;
  mpc8260ads )
    AC_CONFIG_SUBDIRS([mpc8260ads]);;
  ppcn_60x )
    AC_CONFIG_SUBDIRS([ppcn_60x]);;
  psim )
    AC_CONFIG_SUBDIRS([psim]);;
  score603e )
    AC_CONFIG_SUBDIRS([score603e]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
