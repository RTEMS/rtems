## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libbsp-sh-gensh2],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.8])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

## bsp-specific options 
RTEMS_BSPOPTS_SET([CPU_CLOCK_RATE_HZ],[*],[29491200])
RTEMS_BSPOPTS_HELP([CPU_CLOCK_RATE_HZ],[cpu clock rate in HZ])

## Used in scitab/Makefile.am
AC_SUBST(CPU_CLOCK_RATE_HZ)

RTEMS_BSPOPTS_SET([START_HW_INIT],[*],[])
RTEMS_BSPOPTS_HELP([START_HW_INIT],
[If defined, selects whether 'early_hw_init()' is called from 'start.S'; 
 'bsp_hw_init()' is always called from 'bspstart.c'])

RTEMS_BSPOPTS_SET([STANDALONE_EVB],[*],[])
RTEMS_BSPOPTS_HELP([STANDALONE_EVB],
[If defined, compiles code to jump-start from FLASH, without a monitor])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
