## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-libtests],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([cpuuse])
AC_CONFIG_HEADER([config.h])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_RTEMS_TEST_NO_PAUSE

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_PROG_CXX_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_CXX(RTEMS_BSP)
RTEMS_CHECK_CPUOPTS([RTEMS_NETWORKING])
RTEMS_CHECK_CPUOPTS([RTEMS_POSIX_API])

AC_PROG_LN_S
AC_PATH_PROG([PAX],[pax],no)
AC_PATH_PROG([GZIP],[gzip],no)
AC_PATH_PROG([XZ],[xz],no)

AS_IF([test "x$PAX" = "xno"],[
	AC_MSG_ERROR([pax is missing.])
])

AC_CHECK_HEADERS([complex.h])

AM_CONDITIONAL(TARTESTS,test "$as_ln_s" = "ln -s" && test -n "$PAX" && test -n "$GZIP")
AM_CONDITIONAL(TARTEST_XZ,test -n "$XZ")

AM_CONDITIONAL(HAS_CXX,test "$rtems_cv_HAS_CPLUSPLUS" = "yes")
AM_CONDITIONAL([HAS_COMPLEX],[test "$ac_cv_header_complex_h" = yes])
AM_CONDITIONAL(NETTESTS,test "$rtems_cv_RTEMS_NETWORKING" = "yes")
AM_CONDITIONAL(HAS_POSIX,test x"${rtems_cv_RTEMS_POSIX_API}" = x"yes")

# Must match the list in cpukit.
AC_MSG_CHECKING([whether CPU supports libdl])
case $RTEMS_CPU in
  arm | i386 | m68k | mips | moxie | powerpc | sparc)
   TEST_LIBDL=yes ;;
  # bfin has an issue to resolve with libdl. See ticket #2252
  bfin)
   HAVE_LIBDL=no ;;
  # lm32 has an issue to resolve with libdl. See ticket #2283
  lm32)
   HAVE_LIBDL=no ;;
  # v850 has an issue to resolve with libdl. See ticket #2260
  v850)
   HAVE_LIBDL=no ;;
  *)
   TEST_LIBDL=no ;;
esac
AC_MSG_RESULT([$TEST_LIBDL])

AS_IF([test x"$TEST_LIBDL" = x"yes"],[
  AC_CHECK_PROG(RTEMS_LD_CHECK,rtems-ld,yes)
  if test x"$RTEMS_LD_CHECK" != x"yes" ; then
    TEST_LIBDL=no
  fi
  AC_CHECK_PROG(RTEMS_SYMS_CHECK,rtems-syms,yes)
  if test x"$RTEMS_SYMS_CHECK" != x"yes" ; then
    TEST_LIBDL=no
  fi
])

AM_CONDITIONAL(DLTESTS,[test x"$TEST_LIBDL" = x"yes"])

# Must match the list in cpukit.
AC_MSG_CHECKING([whether CPU supports libdebugger])
case $RTEMS_CPU in
  arm | i386)
   TEST_LIBDEBUGGER=yes ;;
  *)
   TEST_LIBDEBUGGER=no ;;
esac
AC_MSG_RESULT([$TEST_LIBDEBUGGER])
AM_CONDITIONAL(DEBUGGERTESTS,[test x"$TEST_LIBDEBUGGER" = x"yes"])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
POSIX/Makefile
block01/Makefile
block02/Makefile
block03/Makefile
block04/Makefile
block05/Makefile
block06/Makefile
block07/Makefile
block08/Makefile
block09/Makefile
block10/Makefile
block11/Makefile
block12/Makefile
block13/Makefile
block14/Makefile
block15/Makefile
block16/Makefile
block17/Makefile
bspcmdline01/Makefile
capture01/Makefile
complex/Makefile
cpuuse/Makefile
crypt01/Makefile
debugger01/Makefile
defaultconfig01/Makefile
devfs01/Makefile
devfs02/Makefile
devfs03/Makefile
devfs04/Makefile
deviceio01/Makefile
devnullfatal01/Makefile
dl01/Makefile
dl02/Makefile
dl03/Makefile
dl04/Makefile
dl05/Makefile
dumpbuf01/Makefile
exit01/Makefile
exit02/Makefile
flashdisk01/Makefile
ftp01/Makefile
gxx01/Makefile
heapwalk/Makefile
i2c01/Makefile
libfdt01/Makefile
malloc02/Makefile
malloc03/Makefile
malloc04/Makefile
malloctest/Makefile
math/Makefile
mathf/Makefile
mathl/Makefile
md501/Makefile
mghttpd01/Makefile
monitor/Makefile
monitor02/Makefile
mouse01/Makefile
networking01/Makefile
newlib01/Makefile
putenvtest/Makefile
pwdgrp01/Makefile
pwdgrp02/Makefile
rbheap01/Makefile
rtmonuse/Makefile
sha/Makefile
shell01/Makefile
sparsedisk01/Makefile
spi01/Makefile
stackchk/Makefile
stackchk01/Makefile
stringto01/Makefile
syscall01/Makefile
tar01/Makefile
tar02/Makefile
tar03/Makefile
termios/Makefile
termios01/Makefile
termios02/Makefile
termios03/Makefile
termios04/Makefile
termios05/Makefile
termios06/Makefile
termios07/Makefile
termios08/Makefile
termios09/Makefile
top/Makefile
tztest/Makefile
uid01/Makefile
utf8proc01/Makefile
])
AC_OUTPUT
