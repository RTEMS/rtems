## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-cpukit-itron],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([include/itronsys])
RTEMS_TOP(../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.7.2])
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
AC_SUBST(INLINEdir)
AM_CONDITIONAL(INLINE,test "$INLINEdir" = "inline" )
AM_CONDITIONAL(MACROS,test "$INLINEdir" = "macros" )

## Needed when using automake for libs/rels
RTEMS_CHECK_NEWLIB

AM_CONFIG_HEADER([src/config.h])

##
## FIXME: We configure and build the macros subdirectory, too.
## Should probably use AM_CONDITIONALs to disable them in future
# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

