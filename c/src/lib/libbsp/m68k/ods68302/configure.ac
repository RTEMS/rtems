## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-m68k-ods68302],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

AC_ARG_ENABLE(ods68302-debug,
[AC_HELP_STRING([--enable-ods68302-debug],
  [whether to support bsp debugging])],
[case $enable_ods68302_debug in
 yes ) ODS68302_DEBUG=yes;;
 * )   ODS68302_DEBUG=no;;
 esac],
[ODS68302_DEBUG=no])

AM_CONDITIONAL(ODS68302_DEBUG,[test "$ODS68302_DEBUG" = "yes"])

# FIXME: Something seem to be very broken inside of this BSP
#        It plays tricks with VARIANT
#        For now, deactivate it.
# BSP_VARIANT=${BSP_VARIANT-bare}
# AC_DEFINE_UNQUOTED([VARIANT],[${BSP_VARIANT}],[missing explanation])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
start/Makefile
startup/Makefile
timer/Makefile
wrapup/Makefile])
AC_OUTPUT
