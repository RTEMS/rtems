#
# $Id$
#

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([install-if-change.in])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(rtems-tools-build,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE
AC_EXEEXT

AC_PROG_CC
AC_CHECK_FUNCS(strerror strtol)

RTEMS_PATH_KSH

AM_CONFIG_HEADER(config.h)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
install-if-change
lock-directory
unlock-directory
])
AC_OUTPUT
