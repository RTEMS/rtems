## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libcpu-i960],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([i960ca])
RTEMS_TOP(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.8.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
AM_PROG_CC_C_O
RTEMS_CANONICALIZE_TOOLS

AM_CONDITIONAL(i960ca, test "$RTEMS_CPU_MODEL" = "i960ca")
AM_CONDITIONAL(i960ka, test "$RTEMS_CPU_MODEL" = "i960ka")
AM_CONDITIONAL(i960rp, test "$RTEMS_CPU_MODEL" = "i960rp")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
i960ca/Makefile
i960ka/Makefile
i960rp/Makefile
])
AC_OUTPUT

