## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-m68k-mvme167],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_NETWORKING 
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

# HACK: Propagate #define mvme167 into bspopts.h
# Used inside ../mvme162/include/mvme16x_hw.h
AC_DEFINE_UNQUOTED([mvme167],[1],
[If building the mvme167 BSP])

RTEMS_BSPOPTS_SET([CD2401_INT_LEVEL],[*],[1])
RTEMS_BSPOPTS_HELP([CD2401_INT_LEVEL],
[Interrupt level for the CD2401(when CD2401_IO_MODE == 1).])

RTEMS_BSPOPTS_SET([CD2401_IO_MODE],[*],[0])
RTEMS_BSPOPTS_HELP([CD2401_IO_MODE],
[0 for polled I/O, 1 for interrupt-driven.])

RTEMS_BSPOPTS_SET([CD2401_USE_TERMIOS],[*],[0])
RTEMS_BSPOPTS_HELP([CD2401_USE_TERMIOS],
[1 for using termios based console.])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
fatal/Makefile
include/Makefile
network/Makefile
start/Makefile
startup/Makefile
timer/Makefile
wrapup/Makefile])
AC_OUTPUT
