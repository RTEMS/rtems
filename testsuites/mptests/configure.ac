## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-mptests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([mp01])
AC_CONFIG_HEADER([config.h])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE


RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

RTEMS_CHECK_CPUOPTS([RTEMS_MULTIPROCESSING])

AM_CONDITIONAL(HAS_MP,test "$rtems_cv_RTEMS_MULTIPROCESSING" = "yes")

MAX_LONG_TEST_DURATION=${MAX_LONG_TEST_DURATION-100}
AC_ARG_VAR([MAX_LONG_TEST_DURATION],
[The longest length of time a "long running" test should run])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
mp01/Makefile
mp01/node1/Makefile
mp01/node2/Makefile
mp02/Makefile
mp02/node1/Makefile
mp02/node2/Makefile
mp03/Makefile
mp03/node1/Makefile
mp03/node2/Makefile
mp04/Makefile
mp04/node1/Makefile
mp04/node2/Makefile
mp05/Makefile
mp05/node1/Makefile
mp05/node2/Makefile
mp06/Makefile
mp06/node1/Makefile
mp06/node2/Makefile
mp07/Makefile
mp07/node1/Makefile
mp07/node2/Makefile
mp08/Makefile
mp08/node1/Makefile
mp08/node2/Makefile
mp09/Makefile
mp09/node1/Makefile
mp09/node2/Makefile
mp10/Makefile
mp10/node1/Makefile
mp10/node2/Makefile
mp11/Makefile
mp11/node1/Makefile
mp11/node2/Makefile
mp12/Makefile
mp12/node1/Makefile
mp12/node2/Makefile
mp13/Makefile
mp13/node1/Makefile
mp13/node2/Makefile
mp14/Makefile
mp14/node1/Makefile
mp14/node2/Makefile
])
AC_OUTPUT
