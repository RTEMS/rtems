## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libcpu-mips],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([clock])
RTEMS_TOP(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.8])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
AM_PROG_CC_C_O
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
shared/Makefile
r46xx/Makefile
timer/Makefile
tx39/Makefile
])
AC_OUTPUT
