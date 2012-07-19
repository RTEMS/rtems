## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-psxtests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([psx01])
AC_CONFIG_HEADER([config.h])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

AM_CONDITIONAL([HAS_NETWORKING],[test "$HAS_NETWORKING" = "yes"])

RTEMS_CHECK_CPUOPTS([RTEMS_POSIX_API])
AM_CONDITIONAL(HAS_POSIX,test x"${rtems_cv_RTEMS_POSIX_API}" = x"yes")

# FIXME: IEEE Std 1003.1-2008 mandates sys/mman.h,
# but newlib doesn't have sys/mman.h
AC_CHECK_HEADERS([sys/mman.h])

# FIXME: newlib should supply declaration of pthread_rwlock_unlock()
AC_CHECK_DECLS([pthread_rwlock_unlock],[],[],[[#include <pthread.h>]])

# FIXME: newlib should supply declaration of pthread_atfork()
AC_CHECK_DECLS([pthread_atfork],[],[],[[#include <pthread.h>]])

# Added to pthreads after initial revision.  May not be in toolset
AC_CHECK_DECLS([pthread_attr_getstack],[],[],[[#include <pthread.h>]])

# Added to pthreads after initial revision.  May not be in toolset
AC_CHECK_DECLS([pthread_attr_setstack],[],[],[[#include <pthread.h>]])

# Added to pthreads after initial revision.  May not be in toolset
AC_CHECK_DECLS([pthread_attr_getguardsize],[],[],[[#include <pthread.h>]])

# Added to pthreads after initial revision.  May not be in toolset
AC_CHECK_DECLS([pthread_attr_setguardsize],[],[],[[#include <pthread.h>]])

# Mandated by POSIX, not declared in some versions of newlib.
AC_CHECK_DECLS([getrusage],,,[#include sys/resource.h])

# FIXME: adjtime is a non-standardized BSD/Linux extension
# RTEMS should not rely on adjtime
AC_CHECK_DECLS([adjtime],[],[],[[#include <sys/time.h>]])

# FIXME: IEEE Std 1003.1-2008 mandates mprotect in sys/mman.h,
# RTEMS provides a stub, despite newlib doesn't have sys/mman.h
AC_CHECK_DECLS([mprotect],[],[],[[#include <sys/mman.h>]])

# FIXME: IEEE Std 1003.1-2008 mandates seteuid in unistd.h
# RTEMS provides it, despite newlib doesn't declare it.
AC_CHECK_DECLS([seteuid],[],[],[#include <unistd.h>])

# FIXME: We should get rid of this. It's a cludge.
AC_CHECK_SIZEOF([off_t])
AC_CHECK_SIZEOF([blksize_t])
AC_CHECK_SIZEOF([blkcnt_t])
AC_CHECK_SIZEOF([time_t])
AC_CHECK_SIZEOF([mode_t])

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
psx14/Makefile
psx15/Makefile
psx16/Makefile
psxaio01/Makefile
psxaio02/Makefile
psxaio03/Makefile
psxalarm01/Makefile
psxautoinit01/Makefile
psxautoinit02/Makefile
psxbarrier01/Makefile
psxcancel/Makefile
psxcancel01/Makefile
psxchroot01/Makefile
psxclassic01/Makefile
psxcleanup/Makefile
psxcleanup01/Makefile
psxclock/Makefile
psxcond01/Makefile
psxconfig01/Makefile
psxeintr_join/Makefile
psxenosys/Makefile
psxfatal01/Makefile
psxfatal02/Makefile
psxfchx01/Makefile
psxfile01/Makefile
psxfile02/Makefile
psxfilelock01/Makefile
psxgetrusage01/Makefile
psxhdrs/Makefile
psxid01/Makefile
psximfs01/Makefile
psximfs02/Makefile
psxintrcritical01/Makefile
psxitimer/Makefile
psxkey01/Makefile
psxkey02/Makefile
psxkey03/Makefile
psxmount/Makefile
psxmsgq01/Makefile
psxmsgq02/Makefile
psxmsgq03/Makefile
psxmsgq04/Makefile
psxmutexattr01/Makefile
psxobj01/Makefile
psxpasswd01/Makefile
psxpasswd02/Makefile
psxpipe01/Makefile
psxreaddir/Makefile
psxrdwrv/Makefile
psxrwlock01/Makefile
psxsem01/Makefile
psxsignal01/Makefile
psxsignal02/Makefile
psxsignal03/Makefile
psxsignal04/Makefile
psxsignal05/Makefile
psxsignal06/Makefile
psxspin01/Makefile
psxspin02/Makefile
psxstack01/Makefile
psxstack02/Makefile
psxstat/Makefile
psxsysconf/Makefile
psxtime/Makefile
psxtimer01/Makefile
psxtimer02/Makefile
psxtimes01/Makefile
psxualarm/Makefile
psxusleep/Makefile
])
AC_OUTPUT
