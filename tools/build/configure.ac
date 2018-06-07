AC_PREREQ([2.69])
AC_INIT([rtems-tools-build],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([install-if-change.in])
RTEMS_TOP(../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.12.2])
AM_MAINTAINER_MODE

RTEMS_PATH_KSH

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([install-if-change],[chmod +x install-if-change])
AC_OUTPUT
