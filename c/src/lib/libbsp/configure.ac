## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([README])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)

# Is there code where there should be for this BSP?
# The bare bsp is a special case as it is not under the RTEMS_CPU path
if test "$RTEMS_BSP_FAMILY" = "bare" ; then
  AC_CONFIG_SUBDIRS([bare])
else
  RTEMS_CPU_SUBDIRS
fi

AM_CONDITIONAL(HAS_MP,test "$HAS_MP" = "yes")

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
shmdr/Makefile
shared/Makefile
shared/gdbstub/Makefile
shared/include/Makefile
])
AC_OUTPUT
