# RTEMS_CHECK_BSPDIR(RTEMS_BSP)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  RTEMS_BSP_ALIAS(ifelse([$1],,[${RTEMS_BSP}],[$1]),bspdir)
  case "$bspdir" in
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
