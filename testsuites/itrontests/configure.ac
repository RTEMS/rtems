## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-tests-itrontests],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([itronhello])
RTEMS_TOP([../..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

AC_SUBST(BARE_CPU_CFLAGS)
AC_SUBST(BARE_CPU_MODEL)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
itronhello/Makefile
itronmbf01/Makefile
itronmbox01/Makefile
itronsem01/Makefile
itrontask01/Makefile
itrontask02/Makefile
itrontask03/Makefile
itrontask04/Makefile
itrontime01/Makefile
])
AC_OUTPUT
