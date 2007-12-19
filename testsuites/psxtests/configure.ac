## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-tests-psxtests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([psx01])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.10])
AM_MAINTAINER_MODE


RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

AM_CONDITIONAL([HAS_NETWORKING],[test "$HAS_NETWORKING" = "yes"])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
psx01/Makefile
psx02/Makefile
psx03/Makefile
psx04/Makefile
psx05/Makefile
psx06/Makefile
psx07/Makefile
psx08/Makefile
psx09/Makefile
psx10/Makefile
psx11/Makefile
psx12/Makefile
psx13/Makefile
psxbarrier01/Makefile
psxcancel/Makefile
psxchroot01/Makefile
psxenosys/Makefile
psxfile01/Makefile
psxhdrs/Makefile
psxmount/Makefile
psxmsgq01/Makefile
psxreaddir/Makefile
psxrdwrv/Makefile
psxrwlock01/Makefile
psxsem01/Makefile
psxsignal01/Makefile
psxspin01/Makefile
psxstat/Makefile
psxsysconf/Makefile
psxtime/Makefile
psxtimer01/Makefile
psxtimer02/Makefile
])
AC_OUTPUT
