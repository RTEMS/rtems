## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libcpu-mips],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([clock])
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

AM_CONDITIONAL(r46xx, test "$RTEMS_CPU_MODEL" = "R4600" \
|| test "$RTEMS_CPU_MODEL" = "R4650" )

AM_CONDITIONAL(tx39, test "$RTEMS_CPU_MODEL" = "tx3904")

AM_CONDITIONAL(tx49, test "$RTEMS_CPU_MODEL" = "tx4925" \
|| test "$RTEMS_CPU_MODEL" = "tx4938" )

AM_CONDITIONAL(rm52xx, test "$RTEMS_CPU_MODEL" = "rm5231")

AM_CONDITIONAL(mongoosev, test "$RTEMS_CPU_MODEL" = "mongoosev")

AM_CONDITIONAL(au1x00, test "$RTEMS_CPU_MODEL" = "au1x00")

RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
