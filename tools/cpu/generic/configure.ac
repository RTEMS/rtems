## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-tools-cpu-generic],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([size_rtems.in])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_PATH_KSH

RTEMS_TOOLPATHS
AC_SUBST(program_prefix)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
size_rtems])
AC_OUTPUT
