dnl $Id$

dnl macro to detect mkdir
AC_DEFUN(RTEMS_PATH_MKDIR,
[AC_PATH_PROG(MKDIR,mkdir)
])

dnl macro to check for mkdir -p
AC_DEFUN(RTEMS_PROG_MKDIR_P,
[
AC_REQUIRE([RTEMS_PATH_MKDIR])
AC_MSG_CHECKING([for working $MKDIR -p])
AC_CACHE_VAL(rtems_cv_prog_mkdir_p,
[rm -rf conftestdata
if $MKDIR -p conftestdata 2>/dev/null ;then
rtems_cv_prog_MKDIR_P="yes"
else
rtems_cv_prog_MKDIR_P="no"
fi])dnl
rm -rf conftestdata
AC_MSG_RESULT($rtems_cv_prog_MKDIR_P)
])

dnl macro to check for mkdir -m 0755
AC_DEFUN(RTEMS_PROG_MKDIR_M,
[
AC_REQUIRE([RTEMS_PATH_MKDIR])
AC_MSG_CHECKING([for working $MKDIR -m 0755])
AC_CACHE_VAL(rtems_cv_prog_MKDIR_P,
[rm -rf conftestdata
if $MKDIR -m 0775 conftestdata 2>/dev/null; then
rtems_cv_prog_MKDIR_M="yes"
else
rtems_cv_prog_MKDIR_M="no"
fi])dnl
rm -rf conftestdata
AC_MSG_RESULT($rtems_cv_prog_MKDIR_M)
])

