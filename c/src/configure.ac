## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([optman])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_ENABLE_CXX
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_RDBG

RTEMS_CANONICAL_TARGET_CPU
RTEMS_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTILIB
RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

RTEMS_CHECK_NETWORKING(RTEMS_BSP)

## Note: the order of the directories below is essential
AC_CONFIG_SUBDIRS([make])

## Configure ${srcdir}/../../cpukit as exec/ if multilibs are enabled
AS_IF([test x"$multilib" = x"no"],[
  RTEMS_CHECK_MULTIPROCESSING
  RTEMS_CONFIG_SUBDIR([exec],[../../cpukit],[--with-project-root="../../"])
  AC_SUBST([exec],[exec])
])

AC_CONFIG_SUBDIRS(optman)
AC_CONFIG_SUBDIRS(lib/libcpu)
AC_CONFIG_SUBDIRS(lib/libbsp)
AC_CONFIG_SUBDIRS(libmisc)

AS_IF([test "$RTEMS_CPU" != "unix"],[
## HACK: Suppress libchip for unix
  AC_CONFIG_SUBDIRS(libchip)
])

if test "$HAS_NETWORKING" = "yes"; then
  AC_CONFIG_SUBDIRS(libnetworking)
  if test "$RTEMS_HAS_RDBG" = "yes"; then
    RTEMS_CHECK_RDBG(RTEMS_BSP)
  fi
fi

if test "$HAS_RDBG" = "yes"; then
  AC_CONFIG_SUBDIRS(librdbg)
fi
AM_CONDITIONAL(HAS_NETWORKING, test "$HAS_NETWORKING" = "yes" )
AM_CONDITIONAL(HAS_RDBG, test "$HAS_RDBG" = "yes" )

if test "$RTEMS_HAS_CPLUSPLUS" = "yes"; then
  AC_CONFIG_SUBDIRS(librtems++)
fi 

AC_CONFIG_SUBDIRS(wrapup tests)

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
