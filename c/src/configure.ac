## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([exec])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_ENABLE_CXX
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_RDBG

RTEMS_CANONICAL_TARGET_CPU
RTEMS_CANONICAL_HOST

AM_INIT_AUTOMAKE(rtems-c-src,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTILIB
RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

RTEMS_CHECK_NETWORKING(RTEMS_BSP)

## Note: the order of the directories below is essential
AC_CONFIG_SUBDIRS(make)

## Exclude exec from building if multilibs are enabled
if test x"$multilib" = x"no"; then
hst_subdirs="$hst_subdirs exec"
fi
hst_subdirs="$hst_subdirs lib"
hst_subdirs="$hst_subdirs lib/libcpu"
hst_subdirs="$hst_subdirs lib/libbsp"
hst_subdirs="$hst_subdirs libfs"
hst_subdirs="$hst_subdirs libmisc"

if test "$RTEMS_CPU" != "unix"; then
## HACK: Suppress libchip for unix
hst_subdirs="$hst_subdirs libchip"
fi

if test "$HAS_NETWORKING" = "yes"; then
  hst_subdirs="$hst_subdirs libnetworking librpc"
  if test "$RTEMS_HAS_RDBG" = "yes"; then
    RTEMS_CHECK_RDBG(RTEMS_BSP)
  fi
fi

if test "$HAS_RDBG" = "yes"; then
  hst_subdirs="$hst_subdirs librdbg"
fi
AM_CONDITIONAL(HAS_NETWORKING, test "$HAS_NETWORKING" = "yes" )
AM_CONDITIONAL(HAS_RDBG, test "$HAS_RDBG" = "yes" )

if test "$RTEMS_HAS_CPLUSPLUS" = "yes"; then
  hst_subdirs="$hst_subdirs librtems++";
fi 

AC_CONFIG_SUBDIRS($hst_subdirs)
AC_SUBST(hst_subdirs)

AC_CONFIG_SUBDIRS(wrapup tests)

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_CONFIG_COMMANDS([default],[[]],[[test -d lib || mkdir lib
]])
AC_OUTPUT
