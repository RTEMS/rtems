## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-exec-score-cpu-unix],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([cpu.c])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSCPU

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
AC_PROG_RANLIB

RTEMS_CHECK_NEWLIB
RTEMS_CHECK_MULTIPROCESSING

# The only use of System V IPC is the UNIX port when multiprocessing.
AS_IF([test "$HAS_MP" = "yes"],
  [RTEMS_CHECK_SYSV_UNIX])

## The code fragment below had been used in tools/cpu/unix/gensize.c.
## FIXME: 
## * The pad very likely is not necessary.
AC_CHECK_SIZEOF([CPU_CONTEXT],[],[
#include <stdio.h>
#include <setjmp.h>

typedef struct {
  jmp_buf     regs;
  int         isr_level;
  int         pad[4]; /* just in case */
} CPU_CONTEXT;
])

AM_CONFIG_HEADER(rtems/score/unixsize.h)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
