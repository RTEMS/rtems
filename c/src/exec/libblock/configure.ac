## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-libblock],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([src/bdbuf.c])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_CANONICAL_TARGET_CPU
RTEMS_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSCPU
RTEMS_CHECK_CPU
RTEMS_PROG_CC_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS
AC_PROG_RANLIB

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
src/Makefile
])
AC_OUTPUT
