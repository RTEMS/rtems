AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-nds-tools],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([runtest])
RTEMS_TOP(../../../../../../..)

# HACK: so we use the default native flags and do not inherit cross cflags
# HACK: I really hope we get a better solution soon. --joel
CFLAGS=
AC_PROG_CC
AC_PROG_CXX

## RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT
RTEMS_TOOLPATHS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
