## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-powerpc-motorola_powerpc,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_ENABLE_NETWORKING

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)
RTEMS_CHECK_NETWORKING
RTEMS_CANONICAL_HOST

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")
AM_CONDITIONAL(need_motorola_powerpc, test "$RTEMS_BSP_FAMILY" = "motorola_powerpc")

AS=$CC
AM_PROG_AS

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
bootloader/Makefile
clock/Makefile
console/Makefile
include/Makefile
irq/Makefile
motorola/Makefile
openpic/Makefile
pci/Makefile
residual/Makefile
start/Makefile
startup/Makefile
vectors/Makefile
wrapup/Makefile])
AC_OUTPUT
