## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-tests-libtests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([cpuuse])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.10])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_RTEMS_TEST_NO_PAUSE

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-fasm])

RTEMS_PROG_CXX_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_CXX(RTEMS_BSP)


AM_CONDITIONAL(HAS_CXX,test "$rtems_cv_HAS_CPLUSPLUS" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
cpuuse/Makefile
malloctest/Makefile
monitor/Makefile
monitor02/Makefile
putenvtest/Makefile
rtems++/Makefile
rtmonuse/Makefile
stackchk/Makefile
termios/Makefile
tztest/Makefile
])
AC_OUTPUT
