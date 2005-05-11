## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libcpu-arm],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([shared])
RTEMS_TOP([../../../../..],[../../..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.9])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
AM_PROG_CC_C_O
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

AM_CONDITIONAL(shared, test "$RTEMS_CPU_MODEL" = "at91rm9200" || \
		       test "$RTEMS_CPU_MODEL" = "mc9328mxl" || \
		       test "$RTEMS_CPU_MODEL" = "s3c2400")
AM_CONDITIONAL(at91rm9200, test "$RTEMS_CPU_MODEL" = "at91rm9200")
AM_CONDITIONAL(mc9328mxl, test "$RTEMS_CPU_MODEL" = "mc9328mxl")
AM_CONDITIONAL(s3c2400, test "$RTEMS_CPU_MODEL" = "s3c2400")

AC_PATH_PROG([AMPOLISH3],[ampolish3],[])
AM_CONDITIONAL([AMPOLISH3],[test x"$USE_MAINTAINER_MODE" = x"yes" \
  && test -n "$AMPOLISH3"])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
at91rm9200/Makefile
mc9328mxl/Makefile
s3c2400/Makefile
])
AC_OUTPUT
