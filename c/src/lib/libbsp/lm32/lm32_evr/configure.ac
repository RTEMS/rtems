## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-lm32-lm32_evr],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([ON_SIMULATOR],[*],[])
RTEMS_BSPOPTS_HELP([ON_SIMULATOR],
[If defined, this indicates the BSP is being built to run on the lm32
 simulator in GDB.  This enables fast idle support which speeds up the
 clock ticks while the idle task is running so time spent in the idle
 task is minimized.  This significantly reduces the wall time required
 to execute the RTEMS test suites.  It also enables a special exit and
 alternate printk support.])

RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_PRESS_KEY_FOR_RESET],
[If defined, print a message and wait until pressed before resetting
 board when application exits.])

RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_RESET_BOARD_AT_EXIT],
[If defined, reset the board when the application exits.])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
