## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libcpu-sparc],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([reg_win])
RTEMS_TOP(../../../../..)
AC_CONFIG_AUX_DIR(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

AM_CONDITIONAL(has_instruction_cache, test "$RTEMS_CPU_MODEL" = "leon1" \
|| test "$RTEMS_CPU_MODEL" = "leon2" )

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
cache/Makefile
reg_win/Makefile
syscall/Makefile])
AC_OUTPUT

