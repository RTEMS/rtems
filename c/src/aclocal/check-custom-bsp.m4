dnl $Id$

AC_DEFUN([RTEMS_CHECK_CUSTOM_BSP],[
AC_REQUIRE([RTEMS_TOP])

AC_MSG_CHECKING([for make/custom/[$]$1.cfg])
AS_IF([test -r "$srcdir/$RTEMS_TOPdir/make/custom/[$]$1.cfg"],
  [AC_MSG_RESULT([yes])],
  [AC_MSG_ERROR([no])])
])
