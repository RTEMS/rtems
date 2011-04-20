## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ([2.68])
AC_INIT([rtems-c-src-tests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([PROBLEMS])
RTEMS_TOP([..],[])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.11.1])
AM_MAINTAINER_MODE

RTEMS_ENABLE_TESTS
RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_PROG_CC

RTEMS_CHECK_CPUOPTS([RTEMS_POSIX_API])
RTEMS_CHECK_CPUOPTS([RTEMS_MULTIPROCESSING])
RTEMS_CHECK_CPUOPTS([RTEMS_NETWORKING])

case $enable_tests in
yes | samples )
  AC_CONFIG_SUBDIRS(samples)
  ;;
esac

if test "$enable_tests" = "yes"; then
  # do functionality tests first, then performance tests
  AC_CONFIG_SUBDIRS(libtests sptests fstests psxtests)
  if test "$rtems_cv_RTEMS_MULTIPROCESSING" = "yes"; then
    AC_CONFIG_SUBDIRS(mptests)
  fi
  # Now do performance tests
  AC_CONFIG_SUBDIRS(tmtests psxtmtests)
fi

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
