## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-tools-cpu],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([generic])
RTEMS_TOP(../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.10])
AM_MAINTAINER_MODE

RTEMS_PATH_KSH

RTEMS_TOOLPATHS
AC_SUBST(program_prefix)

AC_CONFIG_SUBDIRS(generic)
case "$RTEMS_CPU" in
sh ) AC_CONFIG_SUBDIRS(sh);;
esac

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
