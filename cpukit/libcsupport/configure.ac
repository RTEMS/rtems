## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-cpukit-lib],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([src])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSCPU

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
AC_PROG_RANLIB

RTEMS_CHECK_NEWLIB
RTEMS_CHECK_MULTIPROCESSING

AM_CONDITIONAL(UNIX,test x"$RTEMS_CPU" = x"unix")
AM_CONDITIONAL(NEWLIB,test x"$RTEMS_USE_NEWLIB" = x"yes")

AS_IF([test x"$RTEMS_USE_NEWLIB" != x"yes"],[
## Provide sys/cdefs.h only if the host doesn't.
  AC_CHECK_HEADERS([sys/cdefs.h],[NEED_SYS_CDEFS_H=no],[NEED_SYS_CDEFS_H=yes])
# FIXME: These checks are only in here to provide
# configuration-time diagnostics and are not really used.
  AC_CHECK_HEADERS([stdint.h inttypes.h])
],[
## Using newlib, we provide sys/cdefs.h
  NEED_SYS_CDEFS_H=yes
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

AM_CONDITIONAL([NEED_SYS_CDEFS_H],[test x"$NEED_SYS_CDEFS_H" = x"yes"])

AM_CONFIG_HEADER([src/config.h])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
