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

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)

AM_CONDITIONAL(HAS_MP,test "$HAS_MP" = "yes")

# shell/* wants to assign file descriptors to stdio file descriptors.
AC_MSG_CHECKING([for assignable stdio])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM(
    [#include <stdio.h>], 
    [stdin = fopen("/tmp", "r")])],
  [HAVE_ASSIGNABLE_STDIO=yes],
  [HAVE_ASSIGNABLE_STDIO=no])
AC_MSG_RESULT([$HAVE_ASSIGNABLE_STDIO])

AM_CONDITIONAL([RTEMS_LIBSHELL],[test x"$HAVE_ASSIGNABLE_STDIO" = x"yes"])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
cpuuse/Makefile
shell/Makefile
devnull/Makefile
dummy/Makefile
dumpbuf/Makefile
monitor/Makefile
rtmonuse/Makefile
serdbg/Makefile
stackchk/Makefile
capture/Makefile
untar/Makefile
mw-fb/Makefile
wrapup/Makefile
])
AC_OUTPUT
