## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-lib-libbsp-i386-ts_386ex-tools],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([coff2bin])
RTEMS_TOP(../../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_PROJECT_ROOT
RTEMS_TOOLPATHS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
dos_sup/Makefile
ts_1325_ada/Makefile
debug_ada/Makefile
debug_c/Makefile
network_ada/Makefile
network_ada/adasockets/Makefile
network_ada/listener/Makefile
network_ada/tcprelay/Makefile
ts_1325_ada/Makefile
])
AC_OUTPUT
