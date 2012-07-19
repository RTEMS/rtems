## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libcpu-sparc64],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
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

AM_CONDITIONAL(shared, test x"$RTEMS_CPU_MODEL" = x"sun4v" || \
                       test x"$RTEMS_CPU_MODEL" = x"sun4u") 

AM_CONDITIONAL([sun4u],[test x"$RTEMS_CPU_MODEL" = x"sun4u"])
AM_CONDITIONAL([sun4v],[test x"$RTEMS_CPU_MODEL" = x"sun4v"])

## How to add a conditional simexample for the Makefile.am, based on a
## RTEMS_CPU_MODEL defined as simulator.
#AM_CONDITIONAL([simexample],[test x"$RTEMS_CPU_MODEL" = x"simulator"])

RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

