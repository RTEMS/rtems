## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-cpukit],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([score])
RTEMS_TOP(..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc subdir-objects foreign 1.8.1])
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

# Is this a supported CPU?
AC_MSG_CHECKING([if cpu $RTEMS_CPU is supported])
if test -d "$srcdir/score/cpu/$RTEMS_CPU"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_ERROR(no)
fi

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
AM_PROG_CC_C_O
AC_PROG_RANLIB

# FIXME: For the moment, no reason to check for ada
# RTEMS_PROG_GNAT

RTEMS_CHECK_NEWLIB

RTEMS_CHECK_MULTIPROCESSING
RTEMS_CHECK_POSIX_API
RTEMS_CHECK_ITRON_API
RTEMS_CHECK_NETWORKING

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
])

AC_ARG_ENABLE([ada],
[AS_HELP_STRING(--enable-ada,enable ada support)],
[case "${enable_ada}" in
  yes) ;;
  no) ;;
  *)  AC_MSG_ERROR([bad value ${enable_ada} for --enable-ada]) ;;
esac],[enable_ada=no])

# HACK: We should use a feature-based configuration.
AS_IF([test x"${RTEMS_CPU}" = x"unix"],[
# HACK: silently accept --enable-unixlib
  test -n "${enable_unixlib}" || enable_unixlib="yes"
])

# BSD-isms, used throughout the sources
# Not really used by this configure script
# FIXME: They should be eliminated if possible.
AC_CHECK_FUNCS([strsep strcasecmp snprintf])
AC_CHECK_FUNCS([bcopy bcmp])
AC_CHECK_FUNCS([isascii fileno])

cat >>cpuopts.tmp <<\_ACEOF
/* target cpu dependent options file */
/* automatically generated -- DO NOT EDIT!! */
#ifndef __CPU_OPTIONS_h
#define __CPU_OPTIONS_h
_ACEOF

RTEMS_CPUOPT([RTEMS_DEBUG],
  [test x"${enable_rtems_debug}" = x"yes"],
  [1],
  [if RTEMS_DEBUG is enabled])

RTEMS_CPUOPT([RTEMS_INLINES],
  [test x"${enable_rtems_inlines}" = x"yes"],
  [1],
  [if using inlines])

RTEMS_CPUOPT([RTEMS_ITRON_API],
  [test x"$rtems_cv_HAS_ITRON_API" = x"yes"],
  [1],
  [if itron api is supported])

RTEMS_CPUOPT([RTEMS_MULTILIBS],
  [test x"$RTEMS_MULTILIBS" = x"yes"],
  [1],
  [using multilib'ed RTEMS])

RTEMS_CPUOPT([RTEMS_MULTIPROCESSING],
  [test x"$enable_multiprocessing" = x"yes"],
  [1],
  [if multiprocessing is enabled])

RTEMS_CPUOPT([RTEMS_NEWLIB],
  [test x"$RTEMS_USE_NEWLIB" = x"yes"],
  [1],
  [if using newlib])

RTEMS_CPUOPT([RTEMS_POSIX_API],
  [test x"$rtems_cv_HAS_POSIX_API" = x"yes"],
  [1],
  [if posix api is supported])

RTEMS_CPUOPT([RTEMS_UNIX],
  [test x"$RTEMS_CPU" = x"unix"],
  [1],
  [to indicate RTEMS unix])

RTEMS_CPUOPT([RTEMS_UNIXLIB],
  [test x"${enable_unixlib}" = x"yes"],
  [1],
  [to indicate RTEMS using RTEMS's unixlib])

RTEMS_CPUOPT([RTEMS_VERSION],
  [true],
  ["]_RTEMS_VERSION["],
  [RTEMS version string])

cat >>cpuopts.tmp <<\_ACEOF

#endif
_ACEOF

AS_MKDIR_P(score/include/rtems/score)
AS_IF([test -f score/include/rtems/score/cpuopts.h],
[
  AS_IF([cmp -s score/include/rtems/score/cpuopts.h cpuopts.tmp 2>/dev/null],
  [ 
    AC_MSG_NOTICE([score/include/rtems/score/cpuopts.h is unchanged])
    rm -f cpuopts.tmp
  ],[
    AC_MSG_NOTICE([creating score/include/rtems/score/cpuopts.h])
    rm -f score/include/rtems/score/cpuopts.h
    mv cpuopts.tmp score/include/rtems/score/cpuopts.h
  ])
],[
    AC_MSG_NOTICE([creating score/include/rtems/score/cpuopts.h])
    rm -f score/include/rtems/score/cpuopts.h
    mv cpuopts.tmp score/include/rtems/score/cpuopts.h
])

AC_ENABLE_MULTILIB([Makefile],[..])

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

AC_CHECK_HEADERS([sys/cdefs.h],[NEED_SYS_CDEFS_H=no],[NEED_SYS_CDEFS_H=yes])
AC_CHECK_HEADERS([sys/queue.h],[NEED_SYS_QUEUE_H=no],[NEED_SYS_QUEUE_H=yes])

AS_IF([test x"$RTEMS_USE_NEWLIB" != x"yes"],[
## Provide headers only if the host doesn't.
  AC_CHECK_HEADERS([stdint.h],[NEED_STDINT_H=no],[NEED_STDINT_H=yes])
  AC_CHECK_HEADERS([inttypes.h],[NEED_INTTYPES_H=no],[NEED_INTTYPES=yes])
],[
## Using newlib, we provide sys/cdefs.h
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

AM_CONDITIONAL(INLINE,test x"$enable_rtems_inlines" = x"yes" )
AM_CONDITIONAL(MACROS,test x"$enable_rtems_inlines" = x"no" )
AM_CONDITIONAL(HAS_MP,test x"$enable_multiprocessing" = x"yes" )

AM_CONDITIONAL(HAS_POSIX,test x"$rtems_cv_HAS_POSIX_API" = x"yes")
AM_CONDITIONAL(HAS_ITRON,test x"$rtems_cv_HAS_ITRON_API" = x"yes")
AM_CONDITIONAL(HAS_NETWORKING,test x"$HAS_NETWORKING" = x"yes")

# FIXME: All ports should have a libscorecpu.a - But the powerpc doesn't!
AM_CONDITIONAL([LIBSCORECPU],
[test -f "${srcdir}/score/cpu/${RTEMS_CPU}/cpu.c"])

AM_CONDITIONAL(RTEMS_ADA,[test x"$enable_ada" = x"yes" \
  && test x"$HAS_POSIX_API" = x"yes"])

AM_CONDITIONAL([NEED_SYS_CDEFS_H],[test x"$NEED_SYS_CDEFS_H" = x"yes"])
AM_CONDITIONAL([NEED_SYS_QUEUE_H],[test x"$NEED_SYS_QUEUE_H" = x"yes"])
AM_CONDITIONAL([NEED_STDINT_H],[test x"$NEED_STDINT_H" = x"yes"])
AM_CONDITIONAL([NEED_INTTYPES_H],[test x"$NEED_INTTYPES_H" = x"yes"])

AM_CONDITIONAL([LIBSHELL],[test x"$HAVE_ASSIGNABLE_STDIO" = x"yes"])
AM_CONDITIONAL([LIBSERDBG],[test x"$rtems_cv_cc_attribute_weak" = x"yes"])

AC_CONFIG_HEADER(config.h)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([
Makefile
ada/Makefile
rtems/Makefile
sapi/Makefile
score/Makefile
score/cpu/Makefile
posix/Makefile
itron/Makefile
libblock/Makefile
libfs/Makefile
libcsupport/Makefile
libnetworking/Makefile
librpc/Makefile
libmisc/Makefile

wrapup/Makefile
])

AC_OUTPUT
