## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libcpu-sh],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([sh7032])
RTEMS_TOP(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.8.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
AM_PROG_CC_C_O
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

AM_CONDITIONAL([sh7032],[test x"$RTEMS_CPU_MODEL" = x"sh7032"])
AM_CONDITIONAL([sh7045],[test x"$RTEMS_CPU_MODEL" = x"sh7045"])
AM_CONDITIONAL([sh7750],[test x"$RTEMS_CPU_MODEL" = x"sh7750"])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
sh7032/Makefile
sh7045/Makefile
sh7750/Makefile
])
AC_OUTPUT
