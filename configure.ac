## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([c])
RTEMS_TOP([.])

# Abort if trying to build inside of the source tree.
if test -f VERSION; then
  rm -f config.cache config.log confdefs.h
  AC_MSG_ERROR([***]
    [Attempt to build inside of the source tree]
    [Please use a separate build directory, instead] )
fi

AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE(rtems,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

## These option are only in here to let --help report all supported
## options.
RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENABLE_POSIX
RTEMS_ENABLE_ITRON
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_RDBG
RTEMS_ENABLE_INLINES
RTEMS_ENABLE_CXX
RTEMS_ENABLE_GCC28
RTEMS_ENABLE_LIBCDIR
RTEMS_ENABLE_TESTS  
RTEMS_ENABLE_RTEMS_DEBUG
RTEMS_ENABLE_RTEMSBSP(dummy)
RTEMS_ENABLE_MULTILIB

AC_ARG_ENABLE([docs],
  [AC_HELP_STRING([--enable-docs],[enable building documentation
    (default:disabled)])])

if test $host != $build; then
AC_MSG_WARN([]
[*** *** *** WARNING *** *** ***]
[]
[Entering BEYOND BLEEDING EDGE TERRITORY]
[]
[You are trying to build RTEMS Canadian cross]
[If you really mean it, feel free to continue ...]
[*** *** *** *** *** *** *** ***])
fi

## NOTES:
## The tools/*-directories situation is unclear
## * tools/build are host-native tools to be installed on the host.
## * tools/cpu are host-native or host-cross-target-tools
## * tools/update are build-host-native tools

# these tools are built for the build environment
#
RTEMS_BUILD_CONFIG_SUBDIRS([tools/update])

RTEMS_HOST_CONFIG_SUBDIRS([tools/build])
RTEMS_HOST_CONFIG_SUBDIRS([tools/cpu])

AS_IF([test x"${enable_docs}" = x"yes"],
  [RTEMS_HOST_CONFIG_SUBDIRS([doc])])

RTEMS_TARGET_CONFIG_SUBDIRS([c/make])
if test x"$enable_multilib" = x"yes"; then
RTEMS_TARGET_CONFIG_SUBDIRS([c/src/exec])
# FIXME: Not yet
# RTEMS_TARGET_CONFIG_SUBDIRS([c/src/lib])
fi
RTEMS_TARGET_CONFIG_SUBDIRS([c])

AC_CONFIG_FILES([Makefile
tools/Makefile
make/Makefile
make/custom/Makefile
make/Templates/Makefile
make/compilers/Makefile 
])

AC_OUTPUT
