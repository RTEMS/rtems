## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-tests-samples],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([hello])
RTEMS_TOP(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_CXX
RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])

RTEMS_PROG_CXX_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)
RTEMS_CHECK_CXX(RTEMS_BSP)
RTEMS_CHECK_NETWORKING

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")
AM_CONDITIONAL(HAS_CXX,test "$HAS_CPLUSPLUS" = "yes")
AM_CONDITIONAL(HAS_MP,test "$HAS_MP" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
base_sp/Makefile
hello/Makefile
loopback/Makefile
minimum/Makefile
paranoia/Makefile
ticker/Makefile
unlimited/Makefile
base_mp/Makefile
base_mp/node1/Makefile
base_mp/node2/Makefile
cdtest/Makefile
])
AC_OUTPUT
