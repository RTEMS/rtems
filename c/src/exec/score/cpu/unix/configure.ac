## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([cpu.c])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-exec-score-cpu-unix,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSCPU

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_NEWLIB
RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)

# The only use of System V IPC is the UNIX port when multiprocessing.
if test "$HAS_MP" = "yes"; then
  RTEMS_CHECK_SYSV_UNIX
fi

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
rtems/Makefile
rtems/score/Makefile])
AC_OUTPUT
