## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-smptests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([smp01])
AC_CONFIG_HEADER([config.h])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_PROG_CXX_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CPUOPTS([RTEMS_POSIX_API])

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_CPUOPTS([RTEMS_MULTIPROCESSING])
RTEMS_CHECK_CXX(RTEMS_BSP)
RTEMS_CHECK_CPUOPTS([RTEMS_NETWORKING])
RTEMS_CHECK_CPUOPTS([RTEMS_SMP])

AM_CONDITIONAL(SMPTESTS,test "$rtems_cv_RTEMS_SMP" = "yes")

AM_CONDITIONAL([HAS_POSIX],[test x"${rtems_cv_RTEMS_POSIX_API}" = xyes])

# These are SMP related and were added to newlib by RTEMS.
AC_CHECK_DECLS([pthread_attr_setaffinity_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])
AC_CHECK_DECLS([pthread_attr_getaffinity_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])
AC_CHECK_DECLS([pthread_setaffinity_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])
AC_CHECK_DECLS([pthread_getaffinity_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])
AC_CHECK_DECLS([pthread_getattr_np],[],[],[[
  #define _GNU_SOURCE
  #include <pthread.h>]])

# Added to newlib pthreads for RTEMS SMP (np), may not be present
AC_CHECK_HEADERS([sys/cpuset.h])
AM_CONDITIONAL(HAS_CPUSET,test x"${ac_cv_header_sys_cpuset_h}" = x"yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
smp01/Makefile
smp02/Makefile
smp03/Makefile
smp05/Makefile
smp07/Makefile
smp08/Makefile
smp09/Makefile
smpaffinity01/Makefile
smpatomic01/Makefile
smpcache01/Makefile
smpfatal01/Makefile
smpfatal02/Makefile
smpfatal03/Makefile
smpfatal04/Makefile
smpfatal05/Makefile
smpfatal08/Makefile
smpipi01/Makefile
smpload01/Makefile
smplock01/Makefile
smpmigration01/Makefile
smpmigration02/Makefile
smpmrsp01/Makefile
smppsxaffinity01/Makefile
smppsxaffinity02/Makefile
smppsxsignal01/Makefile
smpschedaffinity01/Makefile
smpschedaffinity02/Makefile
smpschedaffinity03/Makefile
smpschedaffinity04/Makefile
smpschedaffinity05/Makefile
smpscheduler01/Makefile
smpscheduler02/Makefile
smpscheduler03/Makefile
smpschedsem01/Makefile
smpsignal01/Makefile
smpswitchextension01/Makefile
smpthreadlife01/Makefile
smpunsupported01/Makefile
smpwakeafter01/Makefile
])
AC_OUTPUT
