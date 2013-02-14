dnl
AC_DEFUN([RTEMS_CHECK_ATOMIC],
[dnl
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl

AC_CACHE_CHECK([whether CPU supports atomic operations],
  [rtems_cv_ATOMIC],[
    AS_IF(
      [test -f "${srcdir}/score/cpu/$RTEMS_CPU/rtems/score/cpuatomic.h"],
      [rtems_cv_ATOMIC="yes"],
      [rtems_cv_ATOMIC="no"])
  ])
])
