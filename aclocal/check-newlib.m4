dnl $Id$

AC_DEFUN(RTEMS_CHECK_NEWLIB,
[dnl
AC_REQUIRE([RTEMS_PROG_CC_FOR_TARGET])dnl
AC_REQUIRE([RTEMS_CANONICALIZE_TOOLS])dnl
AC_CACHE_CHECK([for RTEMS newlib],
  rtems_cv_use_newlib,
  [
dnl some versions of newlib provide not_required_by_rtems
    AC_TRY_LINK(
      [extern void not_required_by_rtems() ;],
      [not_required_by_rtems()],
      rtems_cv_use_newlib="yes")

dnl some versions of newlib provide rtems_provides_crt0()
    if test -z "$rtems_cv_use_newlib"; then
      AC_TRY_LINK(
        [extern void rtems_provides_crt0() ;],
        [rtems_provides_crt0()],
        rtems_cv_use_newlib="yes",
        rtems_cv_use_newlib="no")
    fi
  ])
RTEMS_USE_NEWLIB="$rtems_cv_use_newlib"
AC_SUBST(RTEMS_USE_NEWLIB)

if test x"${RTEMS_USE_NEWLIB}" = x"yes";
then
  AC_DEFINE_UNQUOTED(RTEMS_NEWLIB,1,[if using newlib])
  AC_DEFINE_UNQUOTED(MALLOC_PROVIDED,1,[if malloc is provided])
fi
])
