## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-cpukit-score-cpu-unix],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([cpu.c])
RTEMS_TOP(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.8])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSCPU

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

AC_CONFIG_HEADERS([rtems/score/unixsize-tmp.h],[
echo "/* automatically generated -- DO NOT EDIT!! */" >$tmp/config.h
echo                                                  >>$tmp/config.h
echo "#ifndef __rtems_score_unixsize_h"               >>$tmp/config.h
echo "#define __rtems_score_unixsize_h"               >>$tmp/config.h
echo                                                  >>$tmp/config.h
sed -e '/.*PACKAGE.*/d' rtems/score/unixsize-tmp.h >> $tmp/config.h
echo                                                  >>$tmp/config.h
echo "#endif"                                         >>$tmp/config.h
AS_IF([cmp -s rtems/score/unixsize.h $tmp/config.h 2>/dev/null],
  [AC_MSG_NOTICE([rtems/score/unixsize.h is unchanged])
   rm -f $tmp/config.h],
  [AC_MSG_NOTICE([creating rtems/score/unixsize.h])
   rm -f rtems/score/unixsize.h
   mv $tmp/config.h rtems/score/unixsize.h])
])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
