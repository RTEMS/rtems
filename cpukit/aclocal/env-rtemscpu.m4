dnl $Id$

AC_DEFUN([RTEMS_ENV_RTEMSCPU],
[
  AC_REQUIRE([RTEMS_ENABLE_MULTILIB])

  AC_ARG_VAR([CPU_CFLAGS],[])
  AC_ARG_VAR([CFLAGS_OPTIMIZE_V],
    [CFLAGS for building the optimized variant])
  AC_ARG_VAR([CFLAGS_DEBUG_V],
    [CFLAGS for building the debugging variant])

  AM_CONDITIONAL([MULTILIB],[test x"$multilib" = x"yes"])
])
