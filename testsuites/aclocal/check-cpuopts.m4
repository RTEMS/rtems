# RTEMS_CHECK_CPUOPTS(define)
AC_DEFUN([RTEMS_CHECK_CPUOPTS],
[
AC_CACHE_CHECK(
  [for $1],
  [rtems_cv_$1],
  [AC_COMPILE_IFELSE([
#include <rtems/score/cpuopts.h>
#ifndef $1
choke me
#endif
    ],
    [rtems_cv_$1=yes],
    [rtems_cv_$1=no])
  ])
])
