## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-librdbg],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([src/awk.svc])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_RDBG
RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

AC_CHECK_PROG(RPCGEN,rpcgen,rpcgen,no)
AC_PROG_AWK

if test "$RPCGEN" = "no"; then
  AC_MSG_WARN([** missing rpcgen, required to build librdbg **])
  rpctools="no"
fi
if test "$AWK" = "no"; then
  AC_MSG_WARN([** missing awk, required to build librdbg **])
  rpctools="no"
fi

AM_CONDITIONAL(RPCTOOLS, test "$rpctools" != "no")

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_BSP_ALIAS(${RTEMS_BSP},bspdir)
RTEMS_CHECK_NETWORKING(RTEMS_BSP)
RTEMS_CHECK_RDBG(RTEMS_BSP)

AM_CONDITIONAL(HAS_RDBG,test "$HAS_RDBG" = "yes")

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
include/Makefile
include/rdbg/Makefile
include/rdbg/i386/Makefile
include/rdbg/powerpc/Makefile
include/rdbg/m68k/Makefile
src/Makefile
src/i386/Makefile
src/i386/any/Makefile
src/powerpc/Makefile
src/powerpc/new_exception_processing/Makefile
src/m68k/Makefile
src/m68k/any/Makefile
])
AC_OUTPUT
