# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$RTEMS_BSP_FAMILY" in
  gensh1 )
    AC_CONFIG_SUBDIRS([gensh1]);;
  gensh2 )
    AC_CONFIG_SUBDIRS([gensh2]);;
  gensh4 )
    AC_CONFIG_SUBDIRS([gensh4]);;
  shsim )
    AC_CONFIG_SUBDIRS([shsim]);;
  simsh4 )
    AC_CONFIG_SUBDIRS([simsh4]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
