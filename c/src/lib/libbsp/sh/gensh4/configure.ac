## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-sh-gensh4],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS


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


# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
include/Makefile
start/Makefile
startup/Makefile
hw_init/Makefile
console/Makefile
wrapup/Makefile
])
AC_OUTPUT
