## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-tmtests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([tm01])
AC_CONFIG_HEADER([config.h])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE


RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_RTEMS_TEST_NO_PAUSE

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

OPERATION_COUNT=${OPERATION_COUNT-100}
AC_SUBST(OPERATION_COUNT)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
tmck/Makefile
tmoverhd/Makefile
tm01/Makefile
tm02/Makefile
tm03/Makefile
tm04/Makefile
tm05/Makefile
tm06/Makefile
tm07/Makefile
tm08/Makefile
tm09/Makefile
tm10/Makefile
tm11/Makefile
tm12/Makefile
tm13/Makefile
tm14/Makefile
tm15/Makefile
tm16/Makefile
tm17/Makefile
tm18/Makefile
tm19/Makefile
tm20/Makefile
tm21/Makefile
tm22/Makefile
tm23/Makefile
tm24/Makefile
tm25/Makefile
tm26/Makefile
tm27/Makefile
tm28/Makefile
tm29/Makefile
tm30/Makefile
])
AC_OUTPUT
