## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-libchip],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([rtc])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_NETWORKING(RTEMS_BSP)

# pick up all the Makefiles in required parts of the tree
if test "$RTEMS_CPU" = "unix"; then
## HACK: Suppress libchip for unix
AC_MSG_ERROR(Can't build libchip for $RTEMS_CPU)
fi

AS_IF([test "$host_cpu" = "sparc"],[
  AC_DEFINE_UNQUOTED([CPU_U32_FIX],[1],
    [whether to enable ipalignment work-around])
])

AM_CONDITIONAL(HAS_NETWORKING, test "$HAS_NETWORKING" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
network/Makefile
rtc/Makefile
serial/Makefile
])
AC_OUTPUT
