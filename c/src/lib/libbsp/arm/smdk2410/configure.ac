## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-smdk2410],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_BSPOPTS_SET([ON_SKYEYE],[*],[])
RTEMS_BSPOPTS_HELP([ON_SKYEYE],
[If defined, enable options which optimize executingon the Skyeye
 simulator.  Speed up the clock ticks while the idle task is running so
 time spent in the idle task is minimized.  This significantly reduces
 the wall time required to execute the RTEMS test suites.])

RTEMS_BSPOPTS_SET([CPU_S3C2410],[smdk2410],[1])
RTEMS_BSPOPTS_SET([CPU_S3C2410],[*],[])
RTEMS_BSPOPTS_HELP([CPU_S3C2410],
[If defined, enable optons for the S3C2410 CPU model.])

RTEMS_BSP_CLEANUP_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
