## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([src/imfs/imfs.h])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-libfs,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

AM_CONDITIONAL(UNIX,test x"$RTEMS_CPU" = x"unix")
AM_CONFIG_HEADER(src/imfs/config.h)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
src/Makefile
src/imfs/Makefile
wrapup/Makefile
])
AC_OUTPUT
