## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-samples],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([hello])
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
RTEMS_CHECK_CPUOPTS([RTEMS_MULTIPROCESSING])
RTEMS_CHECK_CXX(RTEMS_BSP)
RTEMS_CHECK_CPUOPTS([RTEMS_NETWORKING])
RTEMS_CHECK_CPUOPTS([RTEMS_SMP])

CXXTESTS=$HAS_CPLUSPLUS
AS_IF([test $HAS_CPLUSPLUS = yes],[
  AC_LANG_PUSH([C++])
  AC_CHECK_HEADERS([cstdio cstdlib iostream],[],[CXXTESTS=no])
  AC_CACHE_CHECK(
  [if iostream supports std:: namespace],
  [rtems_cv_HAVE_IOSTREAM_STD_NAMESPACE],
  [AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
      [[#include <iostream>]],
      [[std::cout << "hello" << std::endl;]])
    ],
    [rtems_cv_HAVE_IOSTREAM_STD_NAMESPACE=yes],
    [rtems_cv_HAVE_IOSTREAM_STD_NAMESPACE=no])
  ])
  AS_IF([test $rtems_cv_HAVE_IOSTREAM_STD_NAMESPACE = no],[CXXTESTS=no])
  AS_IF([test $CXXTESTS = no],[
    AC_MSG_WARN([disabling C++ tests])
  ])
  AC_LANG_POP
])

AM_CONDITIONAL([CXXTESTS],[test $CXXTESTS = "yes"])
AM_CONDITIONAL(NETTESTS,test "$rtems_cv_RTEMS_NETWORKING" = "yes")
AM_CONDITIONAL(HAS_MP,test "$rtems_cv_RTEMS_MULTIPROCESSING" = "yes")
AM_CONDITIONAL(NO_SMP,test "$rtems_cv_RTEMS_SMP" != "yes")
AM_CONDITIONAL(HAS_SMP,test "$rtems_cv_RTEMS_SMP" = "yes")

# FIXME: We should get rid of this. It's a cludge.
AC_CHECK_SIZEOF([time_t])

# BSP Test configuration
RTEMS_TEST_CHECK([base_mp])
RTEMS_TEST_CHECK([base_sp])
RTEMS_TEST_CHECK([capture])
RTEMS_TEST_CHECK([cdtest])
RTEMS_TEST_CHECK([iostream])
RTEMS_TEST_CHECK([fileio])
RTEMS_TEST_CHECK([hello])
RTEMS_TEST_CHECK([loopback])
RTEMS_TEST_CHECK([minimum])
RTEMS_TEST_CHECK([nsecs])
RTEMS_TEST_CHECK([paranoia])
RTEMS_TEST_CHECK([pppd])
RTEMS_TEST_CHECK([ticker])
RTEMS_TEST_CHECK([unlimited])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
