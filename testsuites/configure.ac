## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-tests],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([samples])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_POSIX
RTEMS_ENABLE_ITRON
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_CXX
RTEMS_ENABLE_BARE
RTEMS_ENABLE_TESTS

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)
RTEMS_CHECK_POSIX_API(RTEMS_BSP)
RTEMS_CHECK_ITRON_API(RTEMS_BSP)

AC_CONFIG_SUBDIRS(tools)
AC_CONFIG_SUBDIRS(support)
AC_CONFIG_SUBDIRS(samples)

if test "$tests_enabled" = "yes"; then
  # do functionality tests first, then performance tests
  AC_CONFIG_SUBDIRS(libtests sptests)
  if test "$HAS_MP" = "yes"; then
    AC_CONFIG_SUBDIRS(mptests)
  fi
  if test "$HAS_POSIX_API" = "yes"; then
    AC_CONFIG_SUBDIRS(psxtests)
  fi
  if test "$HAS_ITRON_API" = "yes"; then
    AC_CONFIG_SUBDIRS(itrontests)
  fi
  # Now do performance tests
  AC_CONFIG_SUBDIRS(tmtests)
  if test "$HAS_ITRON_API" = "yes"; then
    AC_CONFIG_SUBDIRS(tmitrontests)
  fi
fi

AC_SUBST(BARE_CPU_CFLAGS)
AC_SUBST(BARE_CPU_MODEL)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
