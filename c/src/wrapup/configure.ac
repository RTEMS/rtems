## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-wrapup],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([../libmisc])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_RDBG

RTEMS_CANONICAL_TARGET_CPU
RTEMS_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTILIB
RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)

# The bare bsp doesn't have libbsp.a
# FIXME: We should use a feature based check, here
AM_CONDITIONAL(HAS_LIBBSP,[! test "$RTEMS_BSP_FAMILY" = "bare"])

RTEMS_CHECK_NETWORKING(RTEMS_BSP)
if test "$HAS_NETWORKING" = "yes"; then
  if test "$RTEMS_HAS_RDBG" = "yes"; then
    RTEMS_CHECK_RDBG(RTEMS_BSP)
  fi
fi

AM_CONDITIONAL(HAS_NETWORKING, test "$HAS_NETWORKING" = "yes" )
AM_CONDITIONAL(HAS_RDBG, test "$HAS_RDBG" = "yes" )

RTEMS_PROJECT_ROOT
RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

AM_CONDITIONAL([MULTILIB],[test x"${enable_multilib}" = x"yes"])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
