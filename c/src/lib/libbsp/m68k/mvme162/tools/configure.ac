## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-m68k-mvme162-tools],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([sload.c])
RTEMS_TOP(../../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

AC_PROG_CC

## FIXME: The checks below are probably incorrect and will not work for
## those actually having sys/vme.h
## Please report any errors related to it, otherwise we will not be able
## to fix building/configuring sload
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h sys/vme.h)
AC_FUNC_MMAP

AM_CONDITIONAL(SLOAD_BUILDABLE, 
test "$ac_cv_header_sys_vme_h" = "yes" \
-a  "$ac_cv_header_unistd_h" = "yes" \
-a "$ac_cv_header_stdc" = "yes" )

RTEMS_PROJECT_ROOT
RTEMS_TOOLPATHS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
