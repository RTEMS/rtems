## Process this file with autoconf to produce a configure script.
##

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-librtems++],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla],[rtems-c-src-librtems++])
AC_CONFIG_SRCDIR([include/rtems++])
RTEMS_TOP(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CXX(RTEMS_BSP)
## check for g++
RTEMS_PROG_CXX_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

AM_CONDITIONAL(HAS_CXX,test "$HAS_CPLUSPLUS" = "yes")

AC_LANG_PUSH(C++)
AC_CHECK_HEADER([cstring],[],[AC_MSG_ERROR([Required header cstring not found])])
AC_CHECK_HEADER([cstdlib],[],[AC_MSG_ERROR([Required header cstdlib not found])])
AC_LANG_POP

RTEMS_PROJECT_ROOT
RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
