AC_PREREQ([2.69])
AC_INIT([rtems-tools-build],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([install-if-change.in])
RTEMS_TOP(../..)

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_CHECK_HEADERS([getopt.h libgen.h])
AC_CHECK_FUNCS(strerror strtol basename)

RTEMS_PATH_KSH

AC_CONFIG_HEADERS([config.h])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([install-if-change],[chmod +x install-if-change])
AC_OUTPUT
