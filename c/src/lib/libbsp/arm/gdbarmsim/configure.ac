## Process this file with autoconf to produce a configure script.
##
## configure.ac,v 1.5 2003/03/11 09:39:07 ralf Exp

AC_PREREQ([2.68])
AC_INIT([rtems-c-src-lib-libbsp-arm-gdbarmsim],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.11.1])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_BSP_CLEANUP_OPTIONS(0, 1)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
