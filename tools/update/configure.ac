#
# $Id$
#

AC_PREREQ(2.57)
AC_INIT([rtems-tools-update],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([rtems-polish.sh])
RTEMS_TOP(../..)

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_PATH_KSH
RTEMS_PATH_PERL

AM_CONDITIONAL(PERL,test -n "$PERL")

RTEMS_TOOLPATHS
# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
