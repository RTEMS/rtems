## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libcpu-sh],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([sh7032])
RTEMS_TOP(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

case "$RTEMS_CPU_MODEL" in
sh7032 ) AC_CONFIG_SUBDIRS([sh7032]);;
sh7045 ) AC_CONFIG_SUBDIRS([sh7045]);;
sh7750 ) AC_CONFIG_SUBDIRS([sh7750]);;
*) AC_MSG_ERROR([Invalid RTEMS_CPU_MODEL]);
esac

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
