## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-librtems++],[_RTEMS_VERSION],[rtems-bugs@rtems.com],[rtems-c-src-librtems++])
AC_CONFIG_SRCDIR([include/rtems++])
RTEMS_TOP(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CXX(RTEMS_BSP)
## check for g++
RTEMS_PROG_CXX_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

AM_CONDITIONAL(HAS_CXX,test "$HAS_CPLUSPLUS" = "yes")

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
