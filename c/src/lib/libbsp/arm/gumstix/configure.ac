## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-gumstix],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_BSPOPTS_SET([ON_SKYEYE],[*],[0])
RTEMS_BSPOPTS_HELP([ON_SKYEYE],
[If defined, enable options which optimize executingon the Skyeye
 simulator.  Speed up the clock ticks while the idle task is running so
 time spent in the idle task is minimized.  This significantly reduces
 the wall time required to execute the RTEMS test suites.])
AM_CONDITIONAL(ON_SKYEYE,test "$ON_SKYEYE" = "1")

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSP_CLEANUP_OPTIONS(1, 1)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
