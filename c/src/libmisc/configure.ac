## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-libmisc],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([dumpbuf])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENABLE_NETWORKING

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)
RTEMS_CHECK_NETWORKING(RTEMS_BSP)

AM_CONDITIONAL(HAS_MP,test "$HAS_MP" = "yes")
AM_CONDITIONAL(HAS_NETWORKING, test "$HAS_NETWORKING" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
cpuuse/Makefile
shell/Makefile
devnull/Makefile
dummy/Makefile
dumpbuf/Makefile
monitor/Makefile
rtmonuse/Makefile
rootfs/Makefile
stackchk/Makefile
capture/Makefile
untar/Makefile
mw-fb/Makefile
wrapup/Makefile
])
AC_OUTPUT
