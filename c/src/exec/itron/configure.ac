## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([include/itronsys])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-exec-itron,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_ENABLE_ITRON
RTEMS_ENABLE_INLINES

RTEMS_ENV_RTEMSCPU

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_ITRON_API(RTEMS_BSP)

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
include/itronsys/Makefile
include/rtems/Makefile
include/rtems/itron/Makefile
inline/Makefile
inline/rtems/Makefile
inline/rtems/itron/Makefile
macros/Makefile
macros/rtems/Makefile
macros/rtems/itron/Makefile
])
AC_OUTPUT

