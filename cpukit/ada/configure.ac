## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-cpukit],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([rtems.adb])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_CANONICAL_TARGET_CPU
RTEMS_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENV_RTEMSCPU
RTEMS_CHECK_CPU

# FIXME: For the moment, no reason to check for ada
# RTEMS_PROG_GNAT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
