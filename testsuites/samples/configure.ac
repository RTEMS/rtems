## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-samples],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([hello])
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

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_CPUOPTS([RTEMS_MULTIPROCESSING])
RTEMS_CHECK_CXX(RTEMS_BSP)
RTEMS_CHECK_CPUOPTS([RTEMS_NETWORKING])

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
AM_CONDITIONAL(MPTESTS,test "$rtems_cv_RTEMS_MULTIPROCESSING" = "yes")

# FIXME: We should get rid of this. It's a cludge.
AC_CHECK_SIZEOF([time_t])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
base_sp/Makefile
hello/Makefile
loopback/Makefile
minimum/Makefile
fileio/Makefile
nsecs/Makefile
paranoia/Makefile
ticker/Makefile
unlimited/Makefile
base_mp/Makefile
base_mp/node1/Makefile
base_mp/node2/Makefile
iostream/Makefile
cdtest/Makefile
pppd/Makefile
capture/Makefile
])
AC_OUTPUT
