## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-hppa1.1-simhppa],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)

AM_CONDITIONAL(HAS_MP,test "$HAS_MP" = "yes")

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

RTEMS_BSPOPTS_SET([SIMHPPA_FAST_IDLE],[*],[1])
RTEMS_BSPOPTS_HELP([SIMHPPA_FAST_IDLE],
[If defined, speed up the clock ticks while the idle task is running so
 time spent in the idle task is minimized.  This significantly reduces 
 the wall time required to execute the RTEMS test suites.])


# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
include/Makefile
shmsupp/Makefile
start/Makefile
startup/Makefile
tty/Makefile
wrapup/Makefile])
AC_OUTPUT
