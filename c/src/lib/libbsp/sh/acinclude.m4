# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  RTEMS_BSP_ALIAS(ifelse([$1],,[${RTEMS_BSP}],[$1]),bspdir)
  case "$bspdir" in
  gensh1 )
    AC_CONFIG_SUBDIRS([gensh1]);;
  gensh2 )
    AC_CONFIG_SUBDIRS([gensh2]);;
  shsim )
    AC_CONFIG_SUBDIRS([shsim]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
