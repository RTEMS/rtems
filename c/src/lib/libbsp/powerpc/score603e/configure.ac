## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-powerpc-score603e,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)
RTEMS_CANONICAL_HOST

## bsp-specific options
CONSOLE_USE_INTERRUPTS=${CONSOLE_USE_INTERRUPTS-0}
AC_DEFINE_UNQUOTED([CONSOLE_USE_INTERRUPTS],
  [$CONSOLE_USE_INTERRUPTS],
  [whether using console interrupts])

AM_CONFIG_HEADER(include/bspopts.h)
RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
PCI_bus/Makefile
clock/Makefile
console/Makefile
include/Makefile
start/Makefile
startup/Makefile
timer/Makefile
tod/Makefile
vectors/Makefile
wrapup/Makefile])
AC_OUTPUT

