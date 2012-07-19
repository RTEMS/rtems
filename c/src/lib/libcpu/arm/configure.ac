## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libcpu-arm],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([shared])
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

AM_CONDITIONAL(shared, test "$RTEMS_CPU_MODEL" = "at91rm9200" || \
		       test "$RTEMS_CPU_MODEL" = "mc9328mxl" || \
		       test "$RTEMS_CPU_MODEL" = "s3c2410" || \
		       test "$RTEMS_CPU_MODEL" = "s3c2400" || \
		       test "$RTEMS_CPU_MODEL" = "pxa255")

AM_CONDITIONAL(at91rm9200, test "$RTEMS_CPU_MODEL" = "at91rm9200")
AM_CONDITIONAL(mc9328mxl, test "$RTEMS_CPU_MODEL" = "mc9328mxl")
AM_CONDITIONAL(lpc22xx, test "$RTEMS_CPU_MODEL" = "lpc22xx")
AM_CONDITIONAL(pxa255, test "$RTEMS_CPU_MODEL" = "pxa255")
AM_CONDITIONAL(s3c24xx, test "$RTEMS_CPU_MODEL" = "s3c2400" ||\
                        test "$RTEMS_CPU_MODEL" = "s3c2410")

RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
