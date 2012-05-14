
# RTEMS_CHECK_CPUOPTS(define)
AC_DEFUN([RTEMS_CHECK_CPUOPTS],
[
sav_CPPFLAGS="$CPPFLAGS"
AS_IF([test "${enable_cpukit_root+set}" = set],[
  CPPFLAGS="$CPPFLAGS -I$cpukit_rootdir/lib/include"
])
AS_IF([test "${enable_project_root+set}" = set],[
  CPPFLAGS="$CPPFLAGS -I$project_rootdir/lib/include"
])
AC_CACHE_CHECK(
  [for $1],
  [rtems_cv_$1],
  [AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <rtems/system.h>
#ifndef $1
choke me
#endif
    ])],
    [rtems_cv_$1=yes],
    [rtems_cv_$1=no])
  ])
CPPFLAGS="$sav_CPPFLAGS"
])
