## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-rhealstone],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([rhtaskswitch])
AC_CONFIG_HEADER([config.h])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_RTEMS_TEST_NO_PAUSE

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
rhtaskswitch/Makefile
rhtaskpreempt/Makefile
rhilatency/Makefile
rhmlatency/Makefile
rhsemshuffle/Makefile
rhdeadlockbrk/Makefile
])
AC_OUTPUT
