#
# $Id$
#

AC_PREREQ(2.57)
AC_INIT([rtems-tools-build],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([install-if-change.in])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_CHECK_FUNCS(strerror strtol)

RTEMS_PATH_KSH

AM_CONFIG_HEADER(config.h)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
install-if-change
])
AC_OUTPUT
