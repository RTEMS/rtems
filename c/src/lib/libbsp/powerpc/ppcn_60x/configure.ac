## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-powerpc-ppcn_60x,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)
RTEMS_CHECK_NETWORKING
RTEMS_CANONICAL_HOST

# Don't build the network directory
# The problem is that the code there has not yet been converted to
# bsd networking
AM_CONDITIONAL(HAS_NETWORKING,false)
# AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([PPC_USE_SPRG],[*],[0])
RTEMS_BSPOPTS_HELP([PPC_USE_SPRG],
[If defined, then the PowerPC specific code in RTEMS will use some
of the special purpose registers to slightly optimize interrupt
response time.  The use of these registers can conflict with
other tools like debuggers.])

RTEMS_BSPOPTS_SET([PPC_USE_DATA_CACHE],[*],[1])
RTEMS_BSPOPTS_HELP([PPC_USE_DATA_CACHE],
[If defined, then the PowerPC specific code in RTEMS will use
 data cache instructions to optimize the context switch code.
 This code can conflict with debuggers or emulators.  It is known
 to break the Corelis PowerPC emulator with at least some combinations
 of PowerPC 603e revisions and emulator versions.
 The BSP actually contains the call that enables this.])

# FIXME: This should be a 1 out of 2 (3??) selection.
RTEMS_BSPOPTS_SET([PPCN_60X_USE_DINK],[*],[1])
RTEMS_BSPOPTS_HELP([PPCN_60X_USE_DINK],
[The Score603e board can be configured with 3 ROM monitors.  Only two
 are appropriate for use with RTEMS.  Set exactly one of these to "1"
 to indicate which ROM monitor is on the board you are using. Corresponds to
PPCN_60X_USE_NONE])

RTEMS_BSPOPTS_SET([PPCN_60X_USE_NONE],[*],[0])
RTEMS_BSPOPTS_HELP([PPCN_60X_USE_NONE],
[The Score603e board can be configured with 3 ROM monitors.  Only two
 are appropriate for use with RTEMS.  Set exactly one of these to "1"
 to indicate which ROM monitor is on the board you are using. Corresponds to
PPCN_60X_USE_DINK])

RTEMS_BSPOPTS_SET([PPC_VECTOR_FILE_BASE],[*],[0x0100])
RTEMS_BSPOPTS_HELP([PPC_VECTOR_FILE_BASE],
[This defines the base address of the exception table.
 NOTE: Vectors are actually at 0xFFF00000 but file starts at offset.])

RTEMS_PROJECT_ROOT
AM_CONFIG_HEADER(include/bspopts.h)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
network/Makefile
nvram/Makefile
pci/Makefile
start/Makefile
startup/Makefile
timer/Makefile
tod/Makefile
universe/Makefile
vectors/Makefile
wrapup/Makefile])

RTEMS_PPC_EXCEPTIONS([old])

AC_OUTPUT
