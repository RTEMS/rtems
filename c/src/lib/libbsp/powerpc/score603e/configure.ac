## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-score603e],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.6])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS


RTEMS_BSPOPTS_SET([CONSOLE_USE_INTERRUPTS],[*],[0])
RTEMS_BSPOPTS_HELP([CONSOLE_USE_INTERRUPTS],
[whether using console interrupts])

RTEMS_BSPOPTS_SET([INITIALIZE_COM_PORTS],[*],[1])
RTEMS_BSPOPTS_HELP([INITIALIZE_COM_PORTS],
[FIXME: Missing explanation])

RTEMS_BSPOPTS_SET([HAS_PMC_PSC8],[*],[0])
RTEMS_BSPOPTS_HELP([HAS_PMC_PSC8],
[FIXME: Missing explanation])

## FIXME: This should be a 1 out of 3 selection
## and is somehow coupled to USE_DINK (cf. include/gen2.h)
RTEMS_BSPOPTS_SET([SCORE603E_USE_SDS],[*],[0])
RTEMS_BSPOPTS_HELP([SCORE603E_USE_SDS],
[use SDS ROM monitor])

RTEMS_BSPOPTS_SET([SCORE603E_OPEN_FIRMWARE],[*],[0])
RTEMS_BSPOPTS_HELP([SCORE603E_OPEN_FIRMWARE],
[use Open Firmware ROM monitor])

RTEMS_BSPOPTS_SET([SCORE603E_USE_NONE],[*],[0])
RTEMS_BSPOPTS_HELP([SCORE603E_USE_NONE],
[use no ROM monitor])

RTEMS_BSPOPTS_SET([SCORE603E_USE_DINK],[*],[1])
RTEMS_BSPOPTS_HELP([SCORE603E_USE_DINK],
[FIXME: Missing explanation.])

RTEMS_BSPOPTS_SET([PPC_USE_SPRG],[*],[0])
RTEMS_BSPOPTS_HELP([PPC_USE_SPRG],
[If defined, then the PowerPC specific code in RTEMS will use some
of the special purpose registers to slightly optimize interrupt
response time.  The use of these registers can conflict with
other tools like debuggers.])

RTEMS_BSPOPTS_SET([PPC_USE_DATA_CACHE],[*],[0])
RTEMS_BSPOPTS_HELP([PPC_USE_DATA_CACHE],
[If defined, then the PowerPC specific code in RTEMS will use
 data cache instructions to optimize the context switch code.
 This code can conflict with debuggers or emulators.  It is known
 to break the Corelis PowerPC emulator with at least some combinations
 of PowerPC 603e revisions and emulator versions.
 The BSP actually contains the call that enables this.])

RTEMS_BSPOPTS_SET([PPC_VECTOR_FILE_BASE],[*],[0x0100])
RTEMS_BSPOPTS_HELP([PPC_VECTOR_FILE_BASE],
[This defines the base address of the exception table.
 NOTE: Vectors are actually at 0xFFF00000 but file starts at offset.])



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

RTEMS_PPC_EXCEPTIONS([old])

AC_OUTPUT
