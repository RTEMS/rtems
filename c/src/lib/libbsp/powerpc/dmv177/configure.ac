## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-powerpc-dmv177,$RTEMS_VERSION,no)
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

## bsp-specific options
RTEMS_BSPOPTS_SET([CONSOLE_USE_INTERRUPTS],[*],[0])
RTEMS_BSPOPTS_HELP([CONSOLE_USE_INTERRUPTS],
[whether using console interrupts])

RTEMS_BSPOPTS_SET([PPC_USE_SPRG],[*],[0])
RTEMS_BSPOPTS_HELP([PPC_USE_SPRG],
[If defined, then the PowerPC specific code in RTEMS will use some
of the special purpose registers to slightly optimize interrupt
response time.  The use of these registers can conflict with
other tools like debuggers.])

RTEMS_BSPOPTS_SET([PPC_USE_DATA_CACHE],[*],[0])
RTEMS_BSPOPTS_HELP([PPC_USE_DATA_CACHE],
[If set != 0, then the PowerPC specific code in RTEMS will use
 data cache instructions to optimize the context switch code.
 This code can conflict with debuggers or emulators.  It is known
 to break the Corelis PowerPC emulator with at least some combinations
 of PowerPC 603e revisions and emulator versions.
 The BSP actually contains the call that enables this.])

RTEMS_BSPOPTS_SET([DMV177_USE_INSTRUCTION_CACHE],[*],[1])
RTEMS_BSPOPTS_HELP([DMV177_USE_INSTRUCTION_CACHE],
[If set != 0, then the PowerPC specific code in RTEMS will use
 data cache instructions to optimize the context switch code.
 This code can conflict with debuggers or emulators.
 The BSP actually contains the call that enables this.])


# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
scv64/Makefile
sonic/Makefile
start/Makefile
startup/Makefile
timer/Makefile
tod/Makefile
wrapup/Makefile
])

RTEMS_PPC_EXCEPTIONS([old])

AC_OUTPUT
