## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-motorola_powerpc],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_CHECK_TOOL(NM,nm,no)
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

RTEMS_BSPOPTS_SET([mvme2100],[mvme2100],[1])
RTEMS_BSPOPTS_SET([mvme2100],[*],[])
RTEMS_BSPOPTS_HELP([mvme2100],
[Defined for MVME2100 -- undefined for others])

RTEMS_BSPOPTS_SET([mpc8240],[mvme2100],[1])
RTEMS_BSPOPTS_SET([mpc8240],[*],[])
RTEMS_BSPOPTS_HELP([mpc8240],
[Defined for boards with MPC8240 -- undefined for others])

BSP_BOOTCARD_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
bootloader/Makefile])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
