#
# $Id$
#

AC_PREREQ(2.59)
AC_INIT([rtems-tools-build],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([install-if-change.in])
RTEMS_TOP(../..)

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.8])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_CHECK_HEADERS([getopt.h])
AC_CHECK_FUNCS(strerror strtol)

RTEMS_PATH_KSH

AC_CONFIG_HEADERS([config.h])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
install-if-change
])
AC_OUTPUT
