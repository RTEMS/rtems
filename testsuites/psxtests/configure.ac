## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-psxtests],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([psx01])
AC_CONFIG_HEADER([config.h])
RTEMS_TOP([../..],[..])
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_PROG_CXX_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_CXX(RTEMS_BSP)
RTEMS_CHECK_CPUOPTS([RTEMS_POSIX_API])
RTEMS_CHECK_CPUOPTS([RTEMS_NETWORKING])

AM_CONDITIONAL(HAS_CXX,test "$rtems_cv_HAS_CPLUSPLUS" = "yes")
AM_CONDITIONAL([HAS_NETWORKING],test "$rtems_cv_RTEMS_NETWORKING" = "yes")
AM_CONDITIONAL([HAS_CPLUSPLUS],[test x"$HAS_CPLUSPLUS" = x"yes"])

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

# Added to newlib pthreads for RTEMS SMP (np), may not be present
AC_CHECK_DECLS([pthread_attr_setaffinity_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])

# Added to newlib pthreads for RTEMS SMP (np), may not be present
AC_CHECK_DECLS([pthread_attr_getaffinity_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])

# Added to newlib pthreads for RTEMS SMP (np), may not be present
AC_CHECK_DECLS([pthread_setaffinity_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])

# Added to newlib pthreads for RTEMS SMP (np), may not be present
AC_CHECK_DECLS([pthread_getaffinity_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])

# Added to newlib pthreads for RTEMS SMP (np), may not be present
AC_CHECK_DECLS([pthread_getattr_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])

# Mandated by POSIX, not declared in some versions of newlib.
AC_CHECK_DECLS([getrusage],,,[#include sys/resource.h])

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

# BSP Test configuration
RTEMS_TEST_CHECK([psx01])
RTEMS_TEST_CHECK([psx02])
RTEMS_TEST_CHECK([psx03])
RTEMS_TEST_CHECK([psx04])
RTEMS_TEST_CHECK([psx05])
RTEMS_TEST_CHECK([psx06])
RTEMS_TEST_CHECK([psx07])
RTEMS_TEST_CHECK([psx08])
RTEMS_TEST_CHECK([psx09])
RTEMS_TEST_CHECK([psx10])
RTEMS_TEST_CHECK([psx11])
RTEMS_TEST_CHECK([psx12])
RTEMS_TEST_CHECK([psx13])
RTEMS_TEST_CHECK([psx14])
RTEMS_TEST_CHECK([psx15])
RTEMS_TEST_CHECK([psx16])
RTEMS_TEST_CHECK([psxaio01])
RTEMS_TEST_CHECK([psxaio02])
RTEMS_TEST_CHECK([psxaio03])
RTEMS_TEST_CHECK([psxalarm01])
RTEMS_TEST_CHECK([psxautoinit01])
RTEMS_TEST_CHECK([psxautoinit02])
RTEMS_TEST_CHECK([psxbarrier01])
RTEMS_TEST_CHECK([psxcancel])
RTEMS_TEST_CHECK([psxcancel01])
RTEMS_TEST_CHECK([psxchroot01])
RTEMS_TEST_CHECK([psxclassic01])
RTEMS_TEST_CHECK([psxcleanup])
RTEMS_TEST_CHECK([psxcleanup01])
RTEMS_TEST_CHECK([psxcleanup02])
RTEMS_TEST_CHECK([psxclock])
RTEMS_TEST_CHECK([psxclock01])
RTEMS_TEST_CHECK([psxclockrealtime01])
RTEMS_TEST_CHECK([psxconcurrency01])
RTEMS_TEST_CHECK([psxcond01])
RTEMS_TEST_CHECK([psxcond02])
RTEMS_TEST_CHECK([psxconfig01])
RTEMS_TEST_CHECK([psxdevctl01])
RTEMS_TEST_CHECK([psxeintr_join])
RTEMS_TEST_CHECK([psxenosys])
RTEMS_TEST_CHECK([psxfatal01])
RTEMS_TEST_CHECK([psxfatal02])
RTEMS_TEST_CHECK([psxfchx01])
RTEMS_TEST_CHECK([psxfile01])
RTEMS_TEST_CHECK([psxfile02])
RTEMS_TEST_CHECK([psxfilelock01])
RTEMS_TEST_CHECK([psxgetattrnp01])
RTEMS_TEST_CHECK([psxgetrusage01])
RTEMS_TEST_CHECK([psxglobalcon01])
RTEMS_TEST_CHECK([psxglobalcon02])
RTEMS_TEST_CHECK([psxid01])
RTEMS_TEST_CHECK([psximfs01])
RTEMS_TEST_CHECK([psximfs02])
RTEMS_TEST_CHECK([psxintrcritical01])
RTEMS_TEST_CHECK([psxitimer])
RTEMS_TEST_CHECK([psxkey01])
RTEMS_TEST_CHECK([psxkey02])
RTEMS_TEST_CHECK([psxkey03])
RTEMS_TEST_CHECK([psxkey04])
RTEMS_TEST_CHECK([psxkey05])
RTEMS_TEST_CHECK([psxkey06])
RTEMS_TEST_CHECK([psxkey07])
RTEMS_TEST_CHECK([psxkey08])
RTEMS_TEST_CHECK([psxkey09])
RTEMS_TEST_CHECK([psxkey10])
RTEMS_TEST_CHECK([psxmmap01])
RTEMS_TEST_CHECK([psxmount])
RTEMS_TEST_CHECK([psxmsgq01])
RTEMS_TEST_CHECK([psxmsgq02])
RTEMS_TEST_CHECK([psxmsgq03])
RTEMS_TEST_CHECK([psxmsgq04])
RTEMS_TEST_CHECK([psxmutexattr01])
RTEMS_TEST_CHECK([psxobj01])
RTEMS_TEST_CHECK([psxonce01])
RTEMS_TEST_CHECK([psxpasswd01])
RTEMS_TEST_CHECK([psxpasswd02])
RTEMS_TEST_CHECK([psxpipe01])
RTEMS_TEST_CHECK([psxrdwrv])
RTEMS_TEST_CHECK([psxreaddir])
RTEMS_TEST_CHECK([psxrwlock01])
RTEMS_TEST_CHECK([psxsem01])
RTEMS_TEST_CHECK([psxshm01])
RTEMS_TEST_CHECK([psxshm02])
RTEMS_TEST_CHECK([psxsignal01])
RTEMS_TEST_CHECK([psxsignal02])
RTEMS_TEST_CHECK([psxsignal03])
RTEMS_TEST_CHECK([psxsignal04])
RTEMS_TEST_CHECK([psxsignal05])
RTEMS_TEST_CHECK([psxsignal06])
RTEMS_TEST_CHECK([psxspin01])
RTEMS_TEST_CHECK([psxstack01])
RTEMS_TEST_CHECK([psxstack02])
RTEMS_TEST_CHECK([psxstat])
RTEMS_TEST_CHECK([psxstrsignal01])
RTEMS_TEST_CHECK([psxsysconf])
RTEMS_TEST_CHECK([psxthreadname01])
RTEMS_TEST_CHECK([psxtime])
RTEMS_TEST_CHECK([psxtimer01])
RTEMS_TEST_CHECK([psxtimer02])
RTEMS_TEST_CHECK([psxtimes01])
RTEMS_TEST_CHECK([psxualarm])
RTEMS_TEST_CHECK([psxusleep])
RTEMS_TEST_CHECK([lib_a])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
