dnl $Id$

dnl check if RTEMS support a cpu
AC_DEFUN(RTEMS_CHECK_CPU,
[dnl
AC_REQUIRE([RTEMS_TOP])
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])

# Is this a supported CPU?
AC_MSG_CHECKING([if cpu $RTEMS_CPU is supported])
if test -d "$srcdir/$RTEMS_TOPdir/cpukit/score/cpu/$RTEMS_CPU"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_ERROR(no)
fi
])dnl

