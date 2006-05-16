# $Id$

AC_DEFUN([RTEMS_AMPOLISH3],
[
AC_PATH_PROG([AMPOLISH3],[ampolish3],[])
AM_CONDITIONAL([AMPOLISH3],
[test x"$USE_MAINTAINER_MODE" = x"yes" \
  && test -n "$AMPOLISH3"])
])

