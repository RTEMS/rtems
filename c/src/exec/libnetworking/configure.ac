## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([arpa])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-libnetworking,$RTEMS_VERSION,no)
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
arpa/Makefile
machine/Makefile
kern/Makefile
sys/Makefile
vm/Makefile
lib/Makefile
libc/Makefile
net/Makefile
netinet/Makefile
nfs/Makefile
pppd/Makefile
rtems/Makefile
rtems_servers/Makefile
rtems_telnetd/Makefile
rtems_webserver/Makefile
wrapup/Makefile
])
AC_OUTPUT
