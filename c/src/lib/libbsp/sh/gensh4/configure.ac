## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-sh-gensh4],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

## bsp-specific options
CPU_CLOCK_RATE_HZ=${CPU_CLOCK_RATE_HZ-96000000}
AC_DEFINE_UNQUOTED(
  [CPU_CLOCK_RATE_HZ],
  [$CPU_CLOCK_RATE_HZ],
  [cpu clock rate in HZ])

#  START_HW_INIT
#	This switch selects whether 'early_hw_init()' is called from
#	'start.S'; 'bsp_hw_init()' is always called from 'bspstart.c'
#
START_HW_INIT=${START_HW_INIT-1}
AC_DEFINE_UNQUOTED(
  [START_HW_INIT],
  [$START_HW_INIT],
  [Whether to call early_hw_init from start.S])

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
