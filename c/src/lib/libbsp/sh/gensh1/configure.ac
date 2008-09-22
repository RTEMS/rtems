## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-sh-gensh1],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

## bsp-specific options
RTEMS_BSPOPTS_SET([CPU_CLOCK_RATE_HZ],[*],[20000000])
RTEMS_BSPOPTS_HELP([CPU_CLOCK_RATE_HZ],[cpu clock rate in HZ])

RTEMS_BSPOPTS_SET([START_HW_INIT],[*],[])
RTEMS_BSPOPTS_HELP([START_HW_INIT],
[If defined, selects whether 'early_hw_init()' is called from 'start.S'; 
 'bsp_hw_init()' is always called from 'bspstart.c'])

## Used in scitab/Makefile.am
AC_SUBST(CPU_CLOCK_RATE_HZ)

RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_PRESS_KEY_FOR_RESET],
[If defined, print a message and wait until pressed before resetting
 board when application exits.])

RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_RESET_BOARD_AT_EXIT],
[If defined, reset the board when the application exits.])

RTEMS_BSP_BOOTCARD_HANDLES_RAM_ALLOCATION

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
