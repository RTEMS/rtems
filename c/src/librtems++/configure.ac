## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-librtems++],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([include/rtems++])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU
RTEMS_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

RTEMS_CHECK_CXX(RTEMS_BSP)
## check for g++
if test "$RTEMS_HAS_CPLUSPLUS" = "yes"; then
  RTEMS_PROG_CXX_FOR_TARGET
fi
RTEMS_CANONICALIZE_TOOLS

AM_CONDITIONAL(HAS_CXX,test "$HAS_CPLUSPLUS" = "yes")

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
include/Makefile
src/Makefile
])
AC_OUTPUT
