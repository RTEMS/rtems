## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.54)
AC_INIT([rtems-cpukit],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([score])
RTEMS_TOP(..)
AC_CONFIG_AUX_DIR(..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTILIB
RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENABLE_POSIX
RTEMS_ENABLE_ITRON
RTEMS_ENABLE_INLINES
RTEMS_ENABLE_RTEMS_DEBUG
RTEMS_ENABLE_NETWORKING

RTEMS_ENV_RTEMSCPU
RTEMS_CHECK_RTEMS_DEBUG

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
AC_PROG_RANLIB

RTEMS_CHECK_NEWLIB

RTEMS_CHECK_MULTIPROCESSING
RTEMS_CHECK_POSIX_API
RTEMS_CHECK_ITRON_API
RTEMS_CHECK_NETWORKING

# If RTEMS macros are enabled, then use them.  Otherwise, use inlines.
AS_IF([test "$RTEMS_USE_MACROS" = "yes"],
  [INLINEdir="macros"],
  [INLINEdir="inline"])
AC_SUBST(INLINEdir)
AM_CONDITIONAL(INLINE,test x"$INLINEdir" = x"inline" )
AM_CONDITIONAL(MACROS,test x"$INLINEdir" = x"macros" )
AM_CONDITIONAL(HAS_MP,test x"$HAS_MP" = x"yes" )

AC_SUBST(RTEMS_VERSION)

AM_CONDITIONAL(HAS_POSIX,test x"$HAS_POSIX_API" = x"yes")
AM_CONDITIONAL(HAS_ITRON,test x"$HAS_ITRON_API" = x"yes")
AM_CONDITIONAL(HAS_NETWORKING,test x"$HAS_NETWORKING" = x"yes")

AS_IF([test x"$HAS_POSIX_API" = x"yes"],
  [AC_CONFIG_SUBDIRS([posix])]
)
AS_IF([test x"$HAS_ITRON_API" = x"yes"],
  [AC_CONFIG_SUBDIRS(itron)]
)
RTEMS_CPU_SUBDIRS([score/cpu])
# FIXME: All ports should have a libscorecpu.a - But the powerpc doesn't!
AM_CONDITIONAL([LIBSCORECPU],
[test -f "${srcdir}/score/cpu/${RTEMS_CPU}/cpu.c"])

AC_CONFIG_SUBDIRS([libcsupport])
AC_CONFIG_SUBDIRS([libblock])
AC_CONFIG_SUBDIRS([libfs])
AS_IF([test x"$HAS_NETWORKING" = x"yes"],[
  AC_CONFIG_SUBDIRS(libnetworking)
## FIXME: Should better use a feature-based test
AC_MSG_CHECKING([whether CPU supports librpc])
  case "$RTEMS_CPU" in
  c4x )  LIBRPC=no;;
  or32 ) LIBRPC=no;;
  * )    LIBRPC=yes;;
  esac
AC_MSG_RESULT([$LIBRPC])
  AS_IF([test "$LIBRPC" = "yes"],
    [AC_CONFIG_SUBDIRS([librpc])
  ])
])
AM_CONDITIONAL(LIBRPC,[test "$LIBRPC" = "yes"])

AC_ARG_ENABLE([ada],
[AC_HELP_STRING([--enable-ada],[enable ada support])],
[case "${enable_ada}" in
  yes) ;;
  no) ;;
  *)  AC_MSG_ERROR([bad value ${enable_ada} for --enable-ada]) ;;
esac],[enable_ada=no])

AS_IF([test x"$enable_ada" = x"yes"],[
  AS_IF([test x"$HAS_POSIX_API" = x"yes"],[
    AC_CONFIG_SUBDIRS([ada])
  ])
])

RTEMS_DEFINE_POSIX_API
RTEMS_DEFINE_ITRON_API
RTEMS_DEFINE_MULTIPROCESSING

# HACK: We should use a feature-based configuration.
AS_IF([test x"${RTEMS_CPU}" = x"unix"],[
  AC_DEFINE_UNQUOTED([RTEMS_UNIX],[1],[to indicate RTEMS unix])
# HACK: silently accept --enable-unixlib
  test -n "${enable_unixlib}" || enable_unixlib="yes"
])

AM_CONDITIONAL(UNIX,[test x"${RTEMS_CPU}" = x"unix"])

AS_IF([test x"${enable_unixlib}" = x"yes"],
  [AC_DEFINE_UNQUOTED([RTEMS_UNIXLIB],[1],
    [to indicate RTEMS using RTEMS's unixlib])]
)


AM_CONFIG_HEADER([score/include/rtems/score/cpuopts-tmp.h],[
echo "/* target cpu dependent options file */"         >$tmp/config.h
echo "/* automatically generated -- DO NOT EDIT!! */" >>$tmp/config.h
echo                                                  >>$tmp/config.h
echo "#ifndef __CPU_OPTIONS_h"                        >>$tmp/config.h
echo "#define __CPU_OPTIONS_h"                        >>$tmp/config.h
echo                                                  >>$tmp/config.h
sed -e '/.*PACKAGE.*/d' score/include/rtems/score/cpuopts-tmp.h >> $tmp/config.h
echo                                                  >>$tmp/config.h
echo "#endif"                                         >>$tmp/config.h
AS_IF([cmp -s score/include/rtems/score/cpuopts.h $tmp/config.h 2>/dev/null],
  [AC_MSG_NOTICE([score/include/rtems/score/cpuopts.h is unchanged])
   rm -f $tmp/config.h],
  [AC_MSG_NOTICE([creating score/include/rtems/score/cpuopts.h])
   rm -f score/include/rtems/score/cpuopts.h
   mv $tmp/config.h score/include/rtems/score/cpuopts.h])
])

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
sapi/Makefile
sapi/include/rtems/sptables.h
score/Makefile
score/cpu/Makefile
wrapup/Makefile
])
AC_OUTPUT
