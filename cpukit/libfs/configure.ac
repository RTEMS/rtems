## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-cpukit-libfs],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([src/imfs/imfs.h])

RTEMS_TOP(../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSCPU

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
AC_PROG_RANLIB

AM_CONDITIONAL(UNIX,test x"$RTEMS_CPU" = x"unix")
AM_CONFIG_HEADER([config.h])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
src/Makefile
src/imfs/Makefile
src/dosfs/Makefile
])
AC_OUTPUT
