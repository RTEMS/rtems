## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-tools-cpu-sh],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([shgen.c])
RTEMS_TOP(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([foreign 1.7.2])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_CHECK_LIB(m,fabs)
AC_CHECK_FUNCS(getopt_long)
AC_CHECK_PROGS(HELP2MAN,help2man)
AM_CONDITIONAL(HELP2MAN,test -n "$HELP2MAN" )

RTEMS_TOOLPATHS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
