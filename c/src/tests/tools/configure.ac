## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([generic])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-tests-tools,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_TOOLPATHS
AC_SUBST(program_prefix)

RTEMS_CONFIG_BUILD_SUBDIRS(generic)
if test -d $srcdir/$RTEMS_CPU; then
AC_CONFIG_SUBDIRS($RTEMS_CPU)
fi

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

RTEMS_OUTPUT_BUILD_SUBDIRS(RTEMS_BUILD_SUBDIRS)
