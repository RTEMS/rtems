## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libbsp-i386-i386ex],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.8])
RTEMS_BSP_CONFIGURE

# RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_PROG_CC_FOR_TARGET([-fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING

## if this is an i386, does gas have good code16 support?
RTEMS_I386_GAS_CODE16
AM_CONDITIONAL(RTEMS_GAS_CODE16,test "$RTEMS_GAS_CODE16" = "yes")

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
