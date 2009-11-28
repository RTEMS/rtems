## Process this file with autoconf to produce a configure script.
##
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-m68k-mcf5206elite-tools],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([runtest])
RTEMS_TOP(../../../../../../..)

# FIXME: This should not be here
AC_ARG_VAR([RTEMS_BSP],[])
RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.10])
AM_MAINTAINER_MODE

RTEMS_PROJECT_ROOT
RTEMS_TOOLPATHS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
