## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-exec-itron],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([include/itronsys])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_ITRON
RTEMS_ENABLE_INLINES

RTEMS_ENV_RTEMSCPU

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
AC_PROG_RANLIB

RTEMS_CHECK_ITRON_API

# If RTEMS macros are enabled, then use them.  Otherwise, use inlines.
if test "$RTEMS_USE_MACROS" = "yes"; then
  INLINEdir="macros"
else
  INLINEdir="inline"
fi
AM_CONDITIONAL(INLINE,test "$INLINEdir" = "inline" )
AM_CONDITIONAL(MACROS,test "$INLINEdir" = "macros" )

## Needed when using automake for libs/rels
RTEMS_CHECK_NEWLIB

AM_CONFIG_HEADER(src/config.h)

##
## FIXME: We configure and build the macros subdirectory, too.
## Should probably use AM_CONDITIONALs to disable them in future
# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
src/Makefile
include/Makefile
inline/Makefile
macros/Makefile
])
AC_OUTPUT

