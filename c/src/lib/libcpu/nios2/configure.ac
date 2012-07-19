## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libcpu-nios2],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
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

# At this time all models should use the shared directory so do this
AM_CONDITIONAL(shared, true)

AC_PATH_PROG([AMPOLISH3],[ampolish3],[])
AM_CONDITIONAL([AMPOLISH3],[test x"$USE_MAINTAINER_MODE" = x"yes" \
  && test -n "$AMPOLISH3"])

RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
