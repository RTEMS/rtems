## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-cpukit-libnetworking],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([arpa])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_NETWORKING

RTEMS_ENV_RTEMSCPU

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
AC_PROG_RANLIB

RTEMS_CHECK_NETWORKING

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
kern/Makefile
lib/Makefile
libc/Makefile
net/Makefile
netinet/Makefile
nfs/Makefile
rtems/Makefile
wrapup/Makefile
])
AC_OUTPUT
