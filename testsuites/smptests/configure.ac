## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-smptests],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([smp01])
AC_CONFIG_HEADER([config.h])
RTEMS_TOP([../..],[..])
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_PROG_CXX_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_CPUOPTS([RTEMS_POSIX_API])
RTEMS_CHECK_CPUOPTS([RTEMS_SMP])

AM_CONDITIONAL(NO_SMP,test "$rtems_cv_RTEMS_SMP" != "yes")
AM_CONDITIONAL(HAS_SMP,test "$rtems_cv_RTEMS_SMP" = "yes")
AM_CONDITIONAL([HAS_POSIX],[test x"${rtems_cv_RTEMS_POSIX_API}" = xyes])

# BSP Test configuration
RTEMS_TEST_CHECK([smp01])
RTEMS_TEST_CHECK([smp02])
RTEMS_TEST_CHECK([smp03])
RTEMS_TEST_CHECK([smp05])
RTEMS_TEST_CHECK([smp07])
RTEMS_TEST_CHECK([smp08])
RTEMS_TEST_CHECK([smp09])
RTEMS_TEST_CHECK([smpaffinity01])
RTEMS_TEST_CHECK([smpatomic01])
RTEMS_TEST_CHECK([smpcache01])
RTEMS_TEST_CHECK([smpcapture01])
RTEMS_TEST_CHECK([smpcapture02])
RTEMS_TEST_CHECK([smpclock01])
RTEMS_TEST_CHECK([smpfatal01])
RTEMS_TEST_CHECK([smpfatal02])
RTEMS_TEST_CHECK([smpfatal03])
RTEMS_TEST_CHECK([smpfatal04])
RTEMS_TEST_CHECK([smpfatal05])
RTEMS_TEST_CHECK([smpfatal06])
RTEMS_TEST_CHECK([smpfatal08])
RTEMS_TEST_CHECK([smpfatal09])
RTEMS_TEST_CHECK([smpipi01])
RTEMS_TEST_CHECK([smpload01])
RTEMS_TEST_CHECK([smplock01])
RTEMS_TEST_CHECK([smpmigration01])
RTEMS_TEST_CHECK([smpmigration02])
RTEMS_TEST_CHECK([smpmrsp01])
RTEMS_TEST_CHECK([smpmulticast01])
RTEMS_TEST_CHECK([smpmutex01])
RTEMS_TEST_CHECK([smpmutex02])
RTEMS_TEST_CHECK([smpopenmp01])
RTEMS_TEST_CHECK([smppsxaffinity01])
RTEMS_TEST_CHECK([smppsxaffinity02])
RTEMS_TEST_CHECK([smppsxmutex01])
RTEMS_TEST_CHECK([smppsxsignal01])
RTEMS_TEST_CHECK([smpschedaffinity01])
RTEMS_TEST_CHECK([smpschedaffinity02])
RTEMS_TEST_CHECK([smpschedaffinity03])
RTEMS_TEST_CHECK([smpschedaffinity04])
RTEMS_TEST_CHECK([smpschedaffinity05])
RTEMS_TEST_CHECK([smpschededf01])
RTEMS_TEST_CHECK([smpschededf02])
RTEMS_TEST_CHECK([smpschededf03])
RTEMS_TEST_CHECK([smpschededf04])
RTEMS_TEST_CHECK([smpschedsem01])
RTEMS_TEST_CHECK([smpscheduler01])
RTEMS_TEST_CHECK([smpscheduler02])
RTEMS_TEST_CHECK([smpscheduler03])
RTEMS_TEST_CHECK([smpscheduler04])
RTEMS_TEST_CHECK([smpscheduler05])
RTEMS_TEST_CHECK([smpscheduler06])
RTEMS_TEST_CHECK([smpscheduler07])
RTEMS_TEST_CHECK([smpsignal01])
RTEMS_TEST_CHECK([smpstrongapa01])
RTEMS_TEST_CHECK([smpswitchextension01])
RTEMS_TEST_CHECK([smpthreadlife01])
RTEMS_TEST_CHECK([smpthreadpin01])
RTEMS_TEST_CHECK([smpunsupported01])
RTEMS_TEST_CHECK([smpwakeafter01])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
