## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-tests-tools-generic],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([sorttimes.in])
RTEMS_TOP(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE
# Override the set of BSPs to be built.

RTEMS_ENV_RTEMSBSP
RTEMS_PROJECT_ROOT

RTEMS_PATH_KSH

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
difftest
sorttimes])
AC_OUTPUT
