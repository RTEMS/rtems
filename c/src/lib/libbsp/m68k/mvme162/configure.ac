## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-m68k-mvme162],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_NETWORKING 

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
network/Makefile
start/Makefile
startup/Makefile
timer/Makefile
tod/Makefile
wrapup/Makefile])
AC_OUTPUT

## consolex/Makefile
