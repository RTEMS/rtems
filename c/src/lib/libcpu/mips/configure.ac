## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libcpu-mips],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([clock])
RTEMS_TOP(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

AM_CONDITIONAL(r46xx, test "$RTEMS_CPU_MODEL" = "R4600" \
|| test "$RTEMS_CPU_MODEL" = "R4650" )

AM_CONDITIONAL(tx39, test "$RTEMS_CPU_MODEL" = "tx3904")

AM_CONDITIONAL(mongoosev, test "$RTEMS_CPU_MODEL" = "mongoosev")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
mongoosev/Makefile
mongoosev/duart/Makefile
mongoosev/vectorisrs/Makefile
shared/Makefile
shared/cache/Makefile
shared/interrupts/Makefile
r46xx/Makefile
r46xx/vectorisrs/Makefile
timer/Makefile
tx39/Makefile
tx39/vectorisrs/Makefile
])
AC_OUTPUT
