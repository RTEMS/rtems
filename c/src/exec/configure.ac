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
AS_IF([test "$RTEMS_USE_MACROS" = "yes"],
  [INLINEdir="macros"],
  [INLINEdir="inline"])

AM_CONDITIONAL(INLINE,test x"$INLINEdir" = x"inline" )
AM_CONDITIONAL(MACROS,test x"$INLINEdir" = x"macros" )
AM_CONDITIONAL(HAS_MP,test x"$HAS_MP" = x"yes" )

AC_SUBST(RTEMS_VERSION)

AM_CONDITIONAL(HAS_POSIX,test x"$HAS_POSIX_API" = x"yes")
AM_CONDITIONAL(HAS_ITRON,test x"$HAS_ITRON_API" = x"yes")

AS_IF([test x"$HAS_POSIX_API" = x"yes"],
  [AC_CONFIG_SUBDIRS([posix])]
)
AS_IF([test x"$HAS_ITRON_API" = x"yes"],
  [AC_CONFIG_SUBDIRS(itron)]
)
RTEMS_CPU_SUBDIRS([score/cpu])

RTEMS_DEFINE_POSIX_API
RTEMS_DEFINE_ITRON_API
RTEMS_DEFINE_MULTIPROCESSING

# HACK: We should use a feature-based configuration.
AS_IF([test x"${RTEMS_CPU}" = x"unix"],[
  AC_DEFINE_UNQUOTED([RTEMS_UNIX],[1],[to indicate RTEMS unix])
# HACK: silently accept --enable-unixlib
  test -n "${enable_unixlib}" || enable_unixlib="yes"
])

AS_IF([test x"${enable_unixlib}" = x"yes"],
  [AC_DEFINE_UNQUOTED([RTEMS_UNIXLIB],[1],
    [to indicate RTEMS using RTEMS's unixlib])]
)

AM_CONFIG_HEADER(score/include/rtems/score/cpuopts-tmp.h)

AC_ENABLE_MULTILIB([Makefile],[..])

## HACK: Add a define to cpuopts.h to indicate using multilibs
## Can be applied to produce compiler errors if using
## multilib-incompatible settings somewhere else (eg. bspopts.h).
AS_IF([test x"${enable_multilib}" = x"yes"],[
  AC_DEFINE_UNQUOTED([RTEMS_MULTILIBS],[1],[using multilib'ed RTEMS])
])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
include/Makefile
rtems/Makefile
rtems/src/Makefile
rtems/include/Makefile
rtems/inline/Makefile
rtems/macros/Makefile
sapi/Makefile
sapi/src/Makefile
sapi/include/Makefile
sapi/include/rtems/sptables.h
sapi/inline/Makefile
sapi/macros/Makefile
score/Makefile
score/cpu/Makefile
score/include/Makefile
score/inline/Makefile
score/macros/Makefile
score/src/Makefile
wrapup/Makefile
])
AC_OUTPUT
