## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-cpukit-libmisc],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([dumpbuf])
RTEMS_TOP(../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSCPU
RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_MULTIPROCESSING

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

# serdbg exploits weak symbols
AC_CACHE_CHECK([whether $CC supports function __attribute__((weak))],
[rtems_cv_cc_attribute_weak],[
  AS_IF([test x"$GCC" = xyes],[
    save_CFLAGS=$CFLAGS
    CFLAGS=-Werror])

  AC_COMPILE_IFELSE([
    AC_LANG_PROGRAM(
    [void myfunc(char c) __attribute__ ((weak));
     void myfunc(char c) {}],
    [])],
    [rtems_cv_cc_attribute_weak=yes],
    [rtems_cv_cc_attribute_weak=no])

  AS_IF([test x"$GCC" = xyes],[
    CFLAGS=$save_CFLAGS])
])

AM_CONDITIONAL([LIBSHELL],[test x"$HAVE_ASSIGNABLE_STDIO" = x"yes"])
AM_CONDITIONAL([LIBSERDBG],[test x"$rtems_cv_cc_attribute_weak" = x"yes"])

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
