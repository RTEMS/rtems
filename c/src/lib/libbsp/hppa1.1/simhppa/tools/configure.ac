## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([print_dump.c])
RTEMS_TOP(../../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-hppa1.1-simhppa-tools,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE
AC_EXEEXT

AC_PROG_CC

AC_CHECK_FUNCS(strerror)
AM_CONFIG_HEADER(config.h)

RTEMS_PROJECT_ROOT
RTEMS_TOOLPATHS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
