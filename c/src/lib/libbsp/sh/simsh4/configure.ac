## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libbsp-sh-simsh4],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.8])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

## bsp-specific options
CPU_CLOCK_RATE_HZ=${CPU_CLOCK_RATE_HZ-10000000}
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

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
console/Makefile
startup/Makefile
clock/Makefile
timer/Makefile
wrapup/Makefile
])
AC_OUTPUT
