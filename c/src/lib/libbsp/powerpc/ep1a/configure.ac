## Process this file with autoconf to produce a configure script.
##
## $Id$

AC_PREREQ([2.68])
AC_INIT([rtems-c-src-lib-libbsp-powerpc-ep1a],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.11.1])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_BSPOPTS_SET([PPC_USE_DATA_CACHE],[*],[0])
RTEMS_BSPOPTS_HELP([PPC_USE_DATA_CACHE],
[If defined, then the PowerPC specific code in RTEMS will use
 data cache instructions to optimize the context switch code.
 This code can conflict with debuggers or emulators.  It is known
 to break the Corelis PowerPC emulator with at least some combinations
 of PowerPC 603e revisions and emulator versions.
 The BSP actually contains the call that enables this.])

RTEMS_BSPOPTS_SET([INSTRUCTION_CACHE_ENABLE],[*],[0])
RTEMS_BSPOPTS_HELP([INSTRUCTION_CACHE_ENABLE],
[If defined, the instruction cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([CONSOLE_USE_INTERRUPTS],[*],[0])
RTEMS_BSPOPTS_HELP([CONSOLE_USE_INTERRUPTS],
[whether using console interrupts])

RTEMS_BSPOPTS_SET([CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK], [*], [1])
RTEMS_BSPOPTS_HELP([CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK],
[If defined then the BSP may reduce the available memory size
initially. This can be useful for debugging (reduce the core
size) or dynamic loading (std gcc text offsets/jumps are < +/-32M).
Note that the policy can still be defined by the application
(see sbrk.c, BSP_sbrk_policy). By undefining
CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK this feature is removed
and a little memory is saved.])

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
