dnl $Id$

AC_DEFUN(RTEMS_CHECK_NEWLIB,
[dnl
AC_REQUIRE([RTEMS_PROG_CC_FOR_TARGET])dnl
AC_REQUIRE([RTEMS_CANONICALIZE_TOOLS])dnl
AC_CACHE_CHECK([for newlib],
  rtems_cv_use_newlib,
  [
    rtems_save_CC=$CC
    CC=$CC_FOR_TARGET

dnl some versions of newlib provide not_required_by_rtems
    AC_TRY_LINK(
      [extern int not_required_by_rtems() ;],
      [not_required_by_rtems()],
      rtems_cv_use_newlib="yes")

dnl older versions of newlib provided rtems_provides_crt0
    if test -z "$rtems_cv_use_newlib"; then
      AC_TRY_LINK(
        [extern int rtems_provides_crt0 ;],
        [rtems_provides_crt0 = 0],
        rtems_cv_use_newlib="yes",
	rtems_cv_use_newlib="no")
    fi
  CC=$rtems_save_CC])
RTEMS_USE_NEWLIB="$rtems_cv_use_newlib"
AC_SUBST(RTEMS_USE_NEWLIB)
])

