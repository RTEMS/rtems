## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-m68k-dmv152],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.6])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS


RTEMS_BSPOPTS_SET([USE_CHANNEL_A],[*],[1])
RTEMS_BSPOPTS_HELP([USE_CHANNEL_A],
[set to 1 to indicate this serial port to be used as RTEMS_CONSOLE])

RTEMS_BSPOPTS_SET([USE_CHANNEL_B],[*],[0])
RTEMS_BSPOPTS_HELP([USE_CHANNEL_B],
[set to 1 to indicate this serial port to be used as RTEMS_CONSOLE])


# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
spurious/Makefile
start/Makefile
startup/Makefile
timer/Makefile
wrapup/Makefile])
AC_OUTPUT
