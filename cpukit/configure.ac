## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-cpukit],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([score])
RTEMS_TOP(..)

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

# FIXME: For the moment, no reason to check for ada
# RTEMS_PROG_GNAT

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
AC_DEFINE(RTEMS_VERSION,["][_RTEMS_VERSION]["],[RTEMS version string])

RTEMS_CPU_SUBDIRS([score/cpu])

AS_IF([test x"$HAS_NETWORKING" = x"yes"],[
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

AC_ARG_ENABLE([ada],
[AC_HELP_STRING([--enable-ada],[enable ada support])],
[case "${enable_ada}" in
  yes) ;;
  no) ;;
  *)  AC_MSG_ERROR([bad value ${enable_ada} for --enable-ada]) ;;
esac],[enable_ada=no])

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

# BSD-isms, used throughout the sources
# Not really used by this configure script
# FIXME: They should be eliminated if possible.
AC_CHECK_FUNCS([strsep strcasecmp snprintf])
AC_CHECK_FUNCS([bcopy bcmp])
AC_CHECK_FUNCS([isascii fileno])

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

# libmisc/shell/* wants to assign file descriptors to stdio file descriptors.
AC_MSG_CHECKING([for assignable stdio])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM(
    [#include <stdio.h>], 
    [stdin = fopen("/tmp", "r")])],
  [HAVE_ASSIGNABLE_STDIO=yes],
  [HAVE_ASSIGNABLE_STDIO=no])
AC_MSG_RESULT([$HAVE_ASSIGNABLE_STDIO])

# libmisc/serdbg exploits weak symbols
AC_CACHE_CHECK([whether $CC supports function __attribute__((weak))],
[rtems_cv_cc_attribute_weak],[
  AS_IF([test x"$GCC" = xyes],[
    save_CFLAGS=$CFLAGS
    CFLAGS=-Werror])

  AC_COMPILE_IFELSE([
    AC_LANG_PROGRAM(
    [void myfunc(char c) __attribute__ ((weak));
     void myfunc(char c) {}],
    [])],
    [rtems_cv_cc_attribute_weak=yes],
    [rtems_cv_cc_attribute_weak=no])

  AS_IF([test x"$GCC" = xyes],[
    CFLAGS=$save_CFLAGS])
])

AS_IF([test x"$RTEMS_USE_NEWLIB" != x"yes"],[
## Provide headers only if the host doesn't.
  AC_CHECK_HEADERS([sys/cdefs.h],[NEED_SYS_CDEFS_H=no],[NEED_SYS_CDEFS_H=yes])
  AC_CHECK_HEADERS([stdint.h],[NEED_STDINT_H=no],[NEED_STDINT_H=yes])
  AC_CHECK_HEADERS([inttypes.h],[NEED_INTTYPES_H=no],[NEED_INTTYPES=yes])
],[
## Using newlib, we provide sys/cdefs.h
  NEED_SYS_CDEFS_H=yes
  NEED_STDINT_H=yes
  NEED_INTTYPES_H=yes
])

# FIXME: These checks are only in here to provide
# configuration-time diagnostics and are not really used.
AC_CHECK_DECLS([_POSIX_LOGIN_NAME_MAX],,,[#include <limits.h>])
AC_CHECK_DECLS([CHAR_BIT],,,[#include <limits.h>])

AS_IF([test x"${RTEMS_USE_NEWLIB}" = x"yes"],[
ac_cv_have_decl_XTABS=yes
ac_cv_have_decl_OLCUC=yes
## SUSV3, but seemingly not available on some platforms (reportedly FreeBSD)
ac_cv_have_decl_ONLRET=yes
ac_cv_have_decl_ONOCR=yes
ac_cv_have_decl_TABDLY=yes
ac_cv_have_decl_OCRNL=yes
ac_cv_have_decl_IUCLC=yes
## SUSV3-XSI extension
ac_cv_have_decl_ECHOPRT=yes
])

# FIXME: Some cruft to work around portability issues with termios.c
## Seemingly not covered by any standard.
AC_CHECK_DECLS([XTABS],,,[#include <termios.h>])
AC_CHECK_DECLS([OLCUC],,,[#include <termios.h>])
## SUSV3, but seemingly not available on some platforms (reportedly FreeBSD)
AC_CHECK_DECLS([ONLRET],,,[#include <termios.h>])
AC_CHECK_DECLS([ONOCR],,,[#include <termios.h>])
AC_CHECK_DECLS([TABDLY],,,[#include <termios.h>])
AC_CHECK_DECLS([OCRNL],,,[#include <termios.h>])
AC_CHECK_DECLS([IUCLC],,,[#include <termios.h>])
## SUSV3-XSI extension
AC_CHECK_DECLS([ECHOPRT],,,[#include <termios.h>])

## BSD-ism, excluded from POSIX, but available on most platforms
AC_CHECK_DECLS([sbrk],,,[#include <unistd.h>])

## Check if libc provides BSD's strlcpy/strlcat
AC_CHECK_FUNCS(strlcpy strlcat)


# ... far too many conditionals ...
AM_CONDITIONAL(LIBRPC,[test "$LIBRPC" = "yes"])
AM_CONDITIONAL(UNIX,[test x"${RTEMS_CPU}" = x"unix"])
AM_CONDITIONAL(NEWLIB,test x"$RTEMS_USE_NEWLIB" = x"yes")

AM_CONDITIONAL(INLINE,test x"$INLINEdir" = x"inline" )
AM_CONDITIONAL(MACROS,test x"$INLINEdir" = x"macros" )
AM_CONDITIONAL(HAS_MP,test x"$HAS_MP" = x"yes" )

AM_CONDITIONAL(HAS_POSIX,test x"$HAS_POSIX_API" = x"yes")
AM_CONDITIONAL(HAS_ITRON,test x"$HAS_ITRON_API" = x"yes")
AM_CONDITIONAL(HAS_NETWORKING,test x"$HAS_NETWORKING" = x"yes")

# FIXME: All ports should have a libscorecpu.a - But the powerpc doesn't!
AM_CONDITIONAL([LIBSCORECPU],
[test -f "${srcdir}/score/cpu/${RTEMS_CPU}/cpu.c"])

AM_CONDITIONAL(RTEMS_ADA,[test x"$enable_ada" = x"yes" \
  && test x"$HAS_POSIX_API" = x"yes"])

AM_CONDITIONAL([NEED_SYS_CDEFS_H],[test x"$NEED_SYS_CDEFS_H" = x"yes"])
AM_CONDITIONAL([NEED_STDINT_H],[test x"$NEED_STDINT_H" = x"yes"])
AM_CONDITIONAL([NEED_INTTYPES_H],[test x"$NEED_INTTYPES_H" = x"yes"])

AM_CONDITIONAL([LIBSHELL],[test x"$HAVE_ASSIGNABLE_STDIO" = x"yes"])
AM_CONDITIONAL([LIBSERDBG],[test x"$rtems_cv_cc_attribute_weak" = x"yes"])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([
Makefile
include/Makefile
ada/Makefile
rtems/Makefile
sapi/Makefile
score/Makefile
score/cpu/Makefile
posix/Makefile
itron/Makefile
libblock/Makefile
libfs/Makefile
libfs/src/Makefile
libfs/src/imfs/Makefile
libfs/src/dosfs/Makefile
libcsupport/Makefile
libnetworking/Makefile
libnetworking/lib/Makefile
libnetworking/libc/Makefile
libnetworking/wrapup/Makefile
libmisc/Makefile
libmisc/cpuuse/Makefile
libmisc/shell/Makefile
libmisc/devnull/Makefile
libmisc/dummy/Makefile
libmisc/dumpbuf/Makefile
libmisc/monitor/Makefile
libmisc/rtmonuse/Makefile
libmisc/serdbg/Makefile
libmisc/stackchk/Makefile
libmisc/capture/Makefile
libmisc/untar/Makefile
libmisc/mw-fb/Makefile
libmisc/wrapup/Makefile

wrapup/Makefile
])

AC_OUTPUT
