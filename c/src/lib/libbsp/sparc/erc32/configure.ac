## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-sparc-erc32,$RTEMS_VERSION,no)
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_NETWORKING

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

## bsp-specific options
CONSOLE_USE_INTERRUPTS=${CONSOLE_USE_INTERRUPTS-0}
AC_DEFINE_UNQUOTED([CONSOLE_USE_INTERRUPTS],
  [$CONSOLE_USE_INTERRUPTS],
  [whether using console interrupts])


# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
erc32sonic/Makefile
gnatsupp/Makefile
include/Makefile
start/Makefile
startup/Makefile
timer/Makefile
wrapup/Makefile])
AC_OUTPUT

RTEMS_OUTPUT_BUILD_SUBDIRS(RTEMS_BUILD_SUBDIRS)
