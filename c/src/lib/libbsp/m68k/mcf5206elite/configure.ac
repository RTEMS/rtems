## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-m68k-mcf5206elite,$RTEMS_VERSION,no)
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_NETWORKING


AC_CONFIG_SUBDIRS(tools)


# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
include/Makefile
start/Makefile
startup/Makefile
console/Makefile
i2c/Makefile
tod/Makefile
nvram/Makefile
wrapup/Makefile])
AC_OUTPUT
