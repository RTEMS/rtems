dnl $Id$
dnl
dnl FIXME: this needs to be reworked

AC_DEFUN(RTEMS_ENABLE_HWAPI,
[dnl
AC_ARG_ENABLE(hwapi, \
[  --enable-hwapi                       enable hardware API library],
[case "${enableval}" in
    yes) RTEMS_HAS_HWAPI=yes ;;
    no)  RTEMS_HAS_HWAPI=no ;;
    *)  AC_MSG_ERROR(bad value ${enableval} for hwapi option) ;;
  esac],[RTEMS_HAS_HWAPI=no])
])dnl
