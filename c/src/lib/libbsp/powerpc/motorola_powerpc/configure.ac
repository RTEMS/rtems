## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-motorola_powerpc],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.8])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
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

## HACK: This should better be propagated through the BSP's bsp.h.
## however, this BSP doesn't have one :(
AC_DEFINE_UNQUOTED([USE_ENHANCED_INTR_API],[1],
 [Use enhanced intr API])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
bootloader/Makefile
wrapup/Makefile])

RTEMS_PPC_EXCEPTIONS([new])

AC_OUTPUT
