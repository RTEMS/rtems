## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([optman])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-exec-posix,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_ENABLE_POSIX
RTEMS_ENABLE_INLINES

RTEMS_ENV_RTEMSCPU

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)
RTEMS_CHECK_POSIX_API(RTEMS_BSP)

# If RTEMS macros are enabled, then use them.  Otherwise, use inlines.
if test "$RTEMS_USE_MACROS" = "yes"; then
  INLINEdir="macros"
else
  INLINEdir="inline"
fi
AM_CONDITIONAL(INLINE,test "$INLINEdir" = "inline" )
AM_CONDITIONAL(MACROS,test "$INLINEdir" = "macros" )
AM_CONDITIONAL(HAS_MP,test "$HAS_MP" = "yes" )

## Needed when using automake for libs/rels
RTEMS_CHECK_NEWLIB

## Refuse to build if bsp does not support libposix
if test "$HAS_POSIX_API" != "yes"; then
AC_MSG_ERROR([BSP $RTEMS_BSP does not support libposix])
fi

AM_CONFIG_HEADER(src/config.h)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
src/Makefile
include/Makefile
inline/Makefile
macros/Makefile
])
AC_OUTPUT
