## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libbsp-m68k-sim68000],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.8])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
startup/Makefile
timer/Makefile
wrapup/Makefile])
AC_OUTPUT
