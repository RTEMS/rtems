dnl Process this file with autoconf to produce a configure script.
dnl
dnl $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-sh-simsh4,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)
RTEMS_CANONICAL_HOST

## bsp-specific options
CPU_CLOCK_RATE_HZ=${CPU_CLOCK_RATE_HZ-10000000}
AC_SUBST(CPU_CLOCK_RATE_HZ)
AC_DEFINE_UNQUOTED(
  [CPU_CLOCK_RATE_HZ],
  [$CPU_CLOCK_RATE_HZ],
  [cpu clock rate in HZ])

AM_CONFIG_HEADER(include/bspopts.h)

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
console/Makefile
include/Makefile
start/Makefile
startup/Makefile
clock/Makefile
timer/Makefile
wrapup/Makefile
])
AC_OUTPUT
