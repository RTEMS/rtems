dnl
dnl RTEMS_BUILD_TOP($1)
dnl
AC_DEFUN([RTEMS_BUILD_TOP],
[dnl
#
# This is a copy of the horrible hack in rtems-top.m4 and it is simpler to
# copy it than attempt to clean this crap up.
#
RTEMS_BUILD_ROOT="${with_rtems_build_top}"
AC_SUBST([RTEMS_BUILD_ROOT])
])dnl
