## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-lib-libbsp-unix-posix],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.6])
RTEMS_BSP_CONFIGURE

RTEMS_ENABLE_CXX


RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_PROG_CXX_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS
RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)

RTEMS_CHECK_CXX

RTEMS_CONFIG_BUILD_SUBDIRS(tools)
AM_CONDITIONAL(HAS_MP,test "$HAS_MP" = "yes")

AM_CONDITIONAL(HAS_CXX,test "$HAS_CPLUSPLUS" = "yes")


# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
shmsupp/Makefile
startup/Makefile
timer/Makefile
wrapup/Makefile])
AC_OUTPUT

RTEMS_OUTPUT_BUILD_SUBDIRS(RTEMS_BUILD_SUBDIRS)
