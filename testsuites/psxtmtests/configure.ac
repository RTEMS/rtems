## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-psxtmtests],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([psxtmmutex02])
RTEMS_TOP([../..],[..])
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
AC_CONFIG_HEADER([config.h])

RTEMS_CHECK_CPUOPTS([RTEMS_POSIX_API])
AM_CONDITIONAL(HAS_POSIX,test x"${rtems_cv_RTEMS_POSIX_API}" = x"yes")

OPERATION_COUNT=${OPERATION_COUNT-100}
AC_SUBST(OPERATION_COUNT)

# BSP Test configuration
RTEMS_TEST_CHECK([psxtmbarrier01])
RTEMS_TEST_CHECK([psxtmbarrier02])
RTEMS_TEST_CHECK([psxtmbarrier03])
RTEMS_TEST_CHECK([psxtmbarrier04])
RTEMS_TEST_CHECK([psxtmbarrierattr01])
RTEMS_TEST_CHECK([psxtmcleanup01])
RTEMS_TEST_CHECK([psxtmclocknanosleep01])
RTEMS_TEST_CHECK([psxtmclocknanosleep02])
RTEMS_TEST_CHECK([psxtmclocknanosleep03])
RTEMS_TEST_CHECK([psxtmcond01])
RTEMS_TEST_CHECK([psxtmcond02])
RTEMS_TEST_CHECK([psxtmcond03])
RTEMS_TEST_CHECK([psxtmcond04])
RTEMS_TEST_CHECK([psxtmcond05])
RTEMS_TEST_CHECK([psxtmcond06])
RTEMS_TEST_CHECK([psxtmcond07])
RTEMS_TEST_CHECK([psxtmcond08])
RTEMS_TEST_CHECK([psxtmcond09])
RTEMS_TEST_CHECK([psxtmcond10])
RTEMS_TEST_CHECK([psxtmkey01])
RTEMS_TEST_CHECK([psxtmkey02])
RTEMS_TEST_CHECK([psxtmmq01])
RTEMS_TEST_CHECK([psxtmmutex01])
RTEMS_TEST_CHECK([psxtmmutex02])
RTEMS_TEST_CHECK([psxtmmutex03])
RTEMS_TEST_CHECK([psxtmmutex04])
RTEMS_TEST_CHECK([psxtmmutex05])
RTEMS_TEST_CHECK([psxtmmutex06])
RTEMS_TEST_CHECK([psxtmmutex07])
RTEMS_TEST_CHECK([psxtmmutexattr01])
RTEMS_TEST_CHECK([psxtmnanosleep01])
RTEMS_TEST_CHECK([psxtmnanosleep02])
RTEMS_TEST_CHECK([psxtmonce01])
RTEMS_TEST_CHECK([psxtmrwlock01])
RTEMS_TEST_CHECK([psxtmrwlock02])
RTEMS_TEST_CHECK([psxtmrwlock03])
RTEMS_TEST_CHECK([psxtmrwlock04])
RTEMS_TEST_CHECK([psxtmrwlock05])
RTEMS_TEST_CHECK([psxtmrwlock06])
RTEMS_TEST_CHECK([psxtmrwlock07])
RTEMS_TEST_CHECK([psxtmmqrcvblock01])
RTEMS_TEST_CHECK([psxtmmqrcvblock02])
RTEMS_TEST_CHECK([psxtmsem01])
RTEMS_TEST_CHECK([psxtmsem02])
RTEMS_TEST_CHECK([psxtmsem03])
RTEMS_TEST_CHECK([psxtmsem04])
RTEMS_TEST_CHECK([psxtmsem05])
RTEMS_TEST_CHECK([psxtmsleep01])
RTEMS_TEST_CHECK([psxtmsleep02])
RTEMS_TEST_CHECK([psxtmthread01])
RTEMS_TEST_CHECK([psxtmthread02])
RTEMS_TEST_CHECK([psxtmthread03])
RTEMS_TEST_CHECK([psxtmthread04])
RTEMS_TEST_CHECK([psxtmthread05])
RTEMS_TEST_CHECK([psxtmthread06])
RTEMS_TEST_CHECK([psxtmthreadattr01])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
