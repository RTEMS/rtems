# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  riscv )
    AC_CONFIG_SUBDIRS([riscv]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
