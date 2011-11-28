dnl $Id: enable-drvmgr.m4,v 1.0

AC_DEFUN([RTEMS_ENABLE_DRVMGR],
[
AC_ARG_ENABLE(drvmgr,
AS_HELP_STRING(--enable-drvmgr,enable drvmgr at startup),
[case "${enableval}" in 
  yes) RTEMS_DRVMGR_STARTUP=yes ;;
  no) RTEMS_DRVMGR_STARTUP=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-drvmgr option) ;;
esac],[RTEMS_DRVMGR_STARTUP=yes]) 
])
