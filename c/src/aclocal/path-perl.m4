dnl $Id$

AC_DEFUN(RTEMS_PATH_PERL,
[
AC_PATH_PROG(PERL,perl)
if test -z "$PERL" ; then
AC_MSG_WARN(
[***]
[   perl was not found]
[   Note: Some tools will not be built.])
fi
])
