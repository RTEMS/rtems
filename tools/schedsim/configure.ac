## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-tools-schedsim],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([rtems])
RTEMS_TOP(../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.10])
AM_MAINTAINER_MODE

RTEMS_PATH_KSH

AC_PROG_RANLIB
AC_PROG_CC
AC_PROG_CXX

RTEMS_TOOLPATHS
AC_SUBST(program_prefix)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([
Makefile
rtems/Makefile
shell/Makefile
shell/shared/Makefile
shell/schedsim_priority/Makefile
])
AC_OUTPUT
