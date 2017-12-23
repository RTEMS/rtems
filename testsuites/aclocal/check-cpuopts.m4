
# RTEMS_CHECK_CPUOPTS(define)
AC_DEFUN([RTEMS_CHECK_CPUOPTS],
[
AC_REQUIRE([RTEMS_BUILD_TOP])
sav_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -I${RTEMS_BUILD_ROOT}/include"
AC_CACHE_CHECK(
  [for $1],
  [rtems_cv_$1],
  [AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <rtems/score/cpuopts.h>
#ifndef $1
choke me
#endif
    ])],
    [rtems_cv_$1=yes],
    [rtems_cv_$1=no])
  ])
CPPFLAGS="$sav_CPPFLAGS"
])
