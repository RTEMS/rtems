## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-libnetworking],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([pppd])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_BARE
RTEMS_ENABLE_POSIX

RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_NETWORKING(RTEMS_BSP)
RTEMS_CHECK_POSIX_API(RTEMS_BSP)

AM_CONDITIONAL(HAS_POSIX,test "$HAS_POSIX_API" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
pppd/Makefile
rtems_servers/Makefile
rtems_telnetd/Makefile
rtems_webserver/Makefile
wrapup/Makefile
])
AC_OUTPUT
