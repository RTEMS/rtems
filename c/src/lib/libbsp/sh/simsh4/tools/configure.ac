## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-sh-simsh4-tools],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([runtest.in])
RTEMS_TOP(../../../../../../..)

# FIXME: This should not be here
AC_ARG_VAR([RTEMS_BSP],[])
RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.6])
AM_MAINTAINER_MODE

RTEMS_PROJECT_ROOT
RTEMS_TOOLPATHS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
runtest
])
AC_OUTPUT
