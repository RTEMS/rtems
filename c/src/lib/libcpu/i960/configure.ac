## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libcpu-i960],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([i960ca])
RTEMS_TOP(../../../../..)

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

AM_CONDITIONAL(i960ca, test "$RTEMS_CPU_MODEL" = "i960ca")
AM_CONDITIONAL(i960ka, test "$RTEMS_CPU_MODEL" = "i960ka")
AM_CONDITIONAL(i960rp, test "$RTEMS_CPU_MODEL" = "i960rp")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
i960ca/Makefile
i960ka/Makefile
i960rp/Makefile])
AC_OUTPUT

