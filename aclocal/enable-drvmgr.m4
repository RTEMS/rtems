AC_DEFUN([RTEMS_ENABLE_DRVMGR],
[
## AC_BEFORE([$0], [RTEMS_CHECK_DRVMGR_STARTUP])dnl

AC_ARG_ENABLE(drvmgr,
[AS_HELP_STRING([--enable-drvmgr],[enable Driver Manager at Startup])],
[case "${enableval}" in 
  yes) RTEMS_DRVMGR_STARTUP=yes ;;
  no) RTEMS_DRVMGR_STARTUP=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-drvmgr option) ;;
esac],[RTEMS_DRVMGR_STARTUP=yes]) 
])
