## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([score])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-exec,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTILIB
RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENABLE_POSIX
RTEMS_ENABLE_ITRON
RTEMS_ENABLE_INLINES
RTEMS_ENABLE_GCC28
RTEMS_ENABLE_RTEMS_DEBUG

RTEMS_ENV_RTEMSCPU
RTEMS_CHECK_RTEMS_DEBUG

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_NEWLIB

RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)
RTEMS_CHECK_POSIX_API(RTEMS_BSP)
RTEMS_CHECK_ITRON_API(RTEMS_BSP)

# If RTEMS macros are enabled, then use them.  Otherwise, use inlines.
if test "$RTEMS_USE_MACROS" = "yes"; then
  INLINEdir="macros"
else
  INLINEdir="inline"
fi
AM_CONDITIONAL(INLINE,test "$INLINEdir" = "inline" )
AM_CONDITIONAL(MACROS,test "$INLINEdir" = "macros" )
AM_CONDITIONAL(HAS_MP,test "$HAS_MP" = "yes" )

AC_SUBST(RTEMS_VERSION)

AM_CONDITIONAL(HAS_POSIX,test "$HAS_POSIX_API" = "yes")
AM_CONDITIONAL(HAS_ITRON,test "$HAS_ITRON_API" = "yes")

if test "$HAS_POSIX_API" = "yes"; then
  AC_CONFIG_SUBDIRS(posix)
fi
if test "$HAS_ITRON_API" = "yes"; then
  AC_CONFIG_SUBDIRS(itron)
fi
AC_CONFIG_SUBDIRS(score/cpu/$RTEMS_CPU)

RTEMS_DEFINE_POSIX_API
RTEMS_DEFINE_ITRON_API
RTEMS_DEFINE_MULTIPROCESSING

# HACK: We should use a feature-based configuration.
if test x"${RTEMS_CPU}" = x"unix"; then
AC_DEFINE_UNQUOTED(RTEMS_UNIX,1,[to indicate RTEMS unix])
fi

AM_CONFIG_HEADER(score/include/rtems/score/cpuopts-tmp.h)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
rtems/Makefile
rtems/src/Makefile
rtems/include/Makefile
rtems/optman/Makefile
rtems/inline/Makefile
rtems/macros/Makefile
sapi/Makefile
sapi/src/Makefile
sapi/include/Makefile
sapi/include/rtems/sptables.h
sapi/inline/Makefile
sapi/macros/Makefile
sapi/optman/Makefile
score/Makefile
score/cpu/Makefile
score/include/Makefile
score/inline/Makefile
score/macros/Makefile
score/src/Makefile
wrapup/Makefile
wrapup/rtems/Makefile
wrapup/itron/Makefile
wrapup/posix/Makefile
])
AC_OUTPUT
