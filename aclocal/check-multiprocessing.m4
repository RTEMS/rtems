dnl
dnl $Id$
dnl

AC_DEFUN(RTEMS_CHECK_MULTIPROCESSING,
[dnl
AC_REQUIRE([RTEMS_TOP])dnl
AC_REQUIRE([RTEMS_CHECK_CPU])dnl
AC_CACHE_CHECK([whether BSP supports multiprocessing],
  rtems_cv_HAS_MP,
  [dnl
    if test -d "$srcdir/${RTEMS_TOPdir}/c/src/lib/libbsp/${RTEMS_CPU}/${$1}/shmsupp"; then
      if test "$RTEMS_HAS_MULTIPROCESSING" = "yes"; then
        rtems_cv_HAS_MP="yes" ;
      else
        rtems_cv_HAS_MP="disabled";
      fi
    else
      rtems_cv_HAS_MP="no";
    fi])
if test "$rtems_cv_HAS_MP" = "yes"; then
HAS_MP="yes"
else
HAS_MP="no"
fi
AC_SUBST(HAS_MP)
])
