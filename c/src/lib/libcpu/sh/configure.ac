## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libcpu-sh],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([sh7032])
RTEMS_TOP([../../../../..],[../../..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
AM_PROG_CC_C_O
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

AM_CONDITIONAL([sh7032],[test x"$RTEMS_CPU_MODEL" = x"sh7032"])
AM_CONDITIONAL([sh7045],[test x"$RTEMS_CPU_MODEL" = x"sh7045"])
AM_CONDITIONAL([sh7750],[test x"$RTEMS_CPU_MODEL" = x"sh7750"])
AM_CONDITIONAL([shgdb],[test x"$RTEMS_CPU_MODEL" = x"shgdb"])

RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
