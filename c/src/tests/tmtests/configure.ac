## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-tests-tmtests],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([tm01])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_RTEMS_TEST_NO_PAUSE
RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

AC_SUBST(BARE_CPU_CFLAGS)
AC_SUBST(BARE_CPU_MODEL)
AC_SUBST(CC_CFLAGS_DEFAULT)
AC_SUBST(CC_CFLAGS_DEBUG_V)
AC_SUBST(CC_CFLAGS_PROFILE_V)
AC_SUBST(CC_LDFLAGS_PROFILE_V)

OPERATION_COUNT=${OPERATION_COUNT-100}
AC_SUBST(OPERATION_COUNT)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
include/Makefile
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
])
AC_OUTPUT
