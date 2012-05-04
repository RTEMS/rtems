AC_DEFUN([RTEMS_CHECK_MULTIPROCESSING],
[dnl
AC_REQUIRE([RTEMS_ENABLE_MULTILIB])dnl
AC_REQUIRE([RTEMS_ENV_RTEMSBSP])dnl
AC_REQUIRE([RTEMS_TOP])dnl
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_MULTIPROCESSING])dnl

AC_CACHE_CHECK([if wanting multiprocessing],
  [rtems_cv_want_multiprocessing],
  [
    AS_IF([test x"$multilib" = x"no"],
      [# no cpukit
       rtems_cv_want_multiprocessing="$enable_multiprocessing"
      ],[
#HACK: Should check for RTEMS_MULTIPROCESSING in cpuopts.h, instead
       rtems_cv_want_multiprocessing="$enable_multiprocessing"
      ])
  ])

AS_IF([test "$rtems_cv_want_multiprocessing" = "yes"],
[
  AC_CACHE_CHECK([whether BSP supports multiprocessing],
  [rtems_cv_HAS_MP],[
    if test -d "$srcdir/${RTEMS_TOPdir}/c/src/lib/libbsp/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}/shmsupp"; then
        rtems_cv_HAS_MP="yes" ;
    else
        rtems_cv_HAS_MP="no";
    fi
  ])
if test $rtems_cv_HAS_MP = "no"; then
AC_MSG_ERROR([multiprocessing requested but not supported])
fi
],[rtems_cv_HAS_MP="no";])

AM_CONDITIONAL(HAS_MP,[test x"$rtems_cv_HAS_MP" = x"yes"])
])
