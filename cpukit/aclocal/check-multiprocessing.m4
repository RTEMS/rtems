dnl
dnl $Id$
dnl

AC_DEFUN([RTEMS_CHECK_MULTIPROCESSING],
[dnl
AC_REQUIRE([RTEMS_ENV_RTEMSCPU])dnl
AC_REQUIRE([RTEMS_ENABLE_MULTIPROCESSING])dnl

AS_IF([test "$enable_multiprocessing" = "yes"],
  [HAS_MP="yes"],
  [HAS_MP="no"])
])

AC_DEFUN(RTEMS_DEFINE_MULTIPROCESSING,
[AC_REQUIRE([RTEMS_CHECK_MULTIPROCESSING])dnl
if test x"${HAS_MP}" = x"yes";
then
  AC_DEFINE_UNQUOTED(RTEMS_MULTIPROCESSING,1,[if multiprocessing is enabled])
fi
])
