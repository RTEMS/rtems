## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-tests-libtests],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([cpuuse])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX
RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_RTEMS_TEST_NO_PAUSE
RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])

RTEMS_PROG_CXX_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_CXX(RTEMS_BSP)

AC_SUBST(BARE_CPU_CFLAGS)
AC_SUBST(BARE_CPU_MODEL)

AM_CONDITIONAL(HAS_CXX,test "$HAS_CPLUSPLUS" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
cpuuse/Makefile
malloctest/Makefile
monitor/Makefile
putenvtest/Makefile
rtems++/Makefile
rtmonuse/Makefile
stackchk/Makefile
termios/Makefile
])
AC_OUTPUT
