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
AS=$CC
AM_PROG_AS

RTEMS_BSPOPTS_SET([PPC_USE_DATA_CACHE],[*],[1])
RTEMS_BSPOPTS_HELP([PPC_USE_DATA_CACHE],
[If defined, then the PowerPC specific code in RTEMS will use
 data cache instructions to optimize the context switch code.
 This code can conflict with debuggers or emulators.  It is known
 to break the Corelis PowerPC emulator with at least some combinations
 of PowerPC 603e revisions and emulator versions.
 The BSP actually contains the call that enables this.])

AM_CONFIG_HEADER(include/bspopts.h)
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

RTEMS_PPC_EXCEPTIONS([new])

AC_OUTPUT
