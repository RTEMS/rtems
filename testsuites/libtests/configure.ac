## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-libtests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
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

AC_PROG_LN_S
AC_PATH_PROG([PAX],[pax])

AC_CHECK_HEADERS([complex.h])

AM_CONDITIONAL(TARTESTS,test "$as_ln_s" = "ln -s" && test -n "$PAX")

AM_CONDITIONAL(HAS_CXX,test "$rtems_cv_HAS_CPLUSPLUS" = "yes")
AM_CONDITIONAL([HAS_COMPLEX],[test "$ac_cv_header_complex_h" = yes])
AM_CONDITIONAL(NETTESTS,test "$rtems_cv_RTEMS_NETWORKING" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
mghttpd01/Makefile
block15/Makefile
block14/Makefile
block13/Makefile
rbheap01/Makefile
syscall01/Makefile
flashdisk01/Makefile
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
bspcmdline01/Makefile
cpuuse/Makefile
devfs01/Makefile
devfs02/Makefile
devfs03/Makefile
devfs04/Makefile
deviceio01/Makefile
devnullfatal01/Makefile
dumpbuf01/Makefile
ftp01/Makefile
gxx01/Makefile
heapwalk/Makefile
malloctest/Makefile
malloc02/Makefile
malloc03/Makefile
malloc04/Makefile
malloc05/Makefile
monitor/Makefile
monitor02/Makefile
mouse01/Makefile
putenvtest/Makefile
rtems++/Makefile
rtmonuse/Makefile
stackchk/Makefile
stackchk01/Makefile
stringto01/Makefile
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
tztest/Makefile
POSIX/Makefile
math/Makefile
mathf/Makefile
mathl/Makefile
complex/Makefile
])
AC_OUTPUT
