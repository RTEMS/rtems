## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-tests-support],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([include])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)

AC_SUBST(BARE_CPU_CFLAGS)
AC_SUBST(BARE_CPU_MODEL)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
include/Makefile
])
AC_OUTPUT
