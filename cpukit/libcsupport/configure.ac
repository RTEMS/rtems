## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-lib],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([src])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSCPU

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
AC_PROG_RANLIB

RTEMS_CHECK_NEWLIB
RTEMS_CHECK_MULTIPROCESSING

AM_CONDITIONAL(UNIX,test x"$RTEMS_CPU" = x"unix")
AM_CONDITIONAL(NEWLIB,test x"$RTEMS_USE_NEWLIB" = x"yes")
AS_IF([test x"$RTEMS_USE_NEWLIB" != x"yes"],[
# FIXME: Currently, these checks are only in here to provide
# configuration-time diagnostics and are not really used.
  AC_CHECK_HEADERS([sys/cdefs.h])
  AC_CHECK_HEADERS([stdint.h inttypes.h])
  AC_CHECK_DECLS([_POSIX_LOGIN_NAME_MAX],,,[#include <limits.h>])
])
AM_CONFIG_HEADER(src/config.h)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
