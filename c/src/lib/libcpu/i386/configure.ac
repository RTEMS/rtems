## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libcpu-i386],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([cpuModel.h])
RTEMS_TOP(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

