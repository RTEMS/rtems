## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-librpc],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([include/rpc])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_NETWORKING

RTEMS_ENV_RTEMSCPU
RTEMS_CHECK_CPU

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
AC_PROG_RANLIB
AC_PROG_RANLIB

RTEMS_CHECK_NETWORKING

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")
AM_CONDITIONAL(rpc_not_supported, test "$RTEMS_CPU" = "c4x")

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
include/Makefile
include/rpc/Makefile
include/rpcsvc/Makefile
src/Makefile
src/xdr/Makefile
src/rpc/Makefile
src/rpc/PSD.doc/Makefile
])
AC_OUTPUT
