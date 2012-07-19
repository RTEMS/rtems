## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-tools-cpu-sh],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([shgen.c])
RTEMS_TOP(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([foreign 1.12.2])
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
