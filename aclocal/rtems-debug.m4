## $Id$

AC_DEFUN(RTEMS_ENABLE_RTEMS_DEBUG,
[
AC_ARG_ENABLE(rtems-debug,
[  --enable-rtems-debug                 enable RTEMS_DEBUG],
[case "${enableval}" in
  yes) RTEMS_DEBUG=yes ;;
  no) RTEMS_DEBUG=no ;;
  *) AC_MSG_ERROR([bad value ${enableval} for RTEMS_DEBUG]) ;;
esac],[RTEMS_DEBUG=no])
])

AC_DEFUN(RTEMS_CHECK_RTEMS_DEBUG,
[AC_REQUIRE([RTEMS_ENABLE_RTEMS_DEBUG])
if test x"${RTEMS_DEBUG}" = x"yes";
then
  AC_DEFINE_UNQUOTED(RTEMS_DEBUG,1,[if RTEMS_DEBUG is enabled])
fi
])

