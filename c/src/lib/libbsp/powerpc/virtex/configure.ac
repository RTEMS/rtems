dnl Process this file with autoconf to produce a configure script.
dnl
dnl $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-virtex],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.9])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([PPC_USE_SPRG],[*],[1])
RTEMS_BSPOPTS_HELP([PPC_USE_SPRG],
[If defined, then the PowerPC specific code in RTEMS will use some
 of the special purpose registers to slightly optimize interrupt
 response time.  The use of these registers can conflict with
 other tools like debuggers.])

RTEMS_BSPOPTS_SET([PPC_VECTOR_FILE_BASE],[*],[0x0100])
RTEMS_BSPOPTS_HELP([PPC_VECTOR_FILE_BASE],
[This defines the base address of the exception table.
 NOTE: Vectors are actually at 0xFFF00000 but file starts at offset.])

RTEMS_BSPOPTS_SET([RTEMS_XPARAMETERS_H],[*],[\<xparameters_dflt.h\>])
RTEMS_BSPOPTS_HELP([RTEMS_XPARAMETERS_H],
[This defines the location of the hardware specific "xparameters.h" file.
 in the file system. Specify an absolute path. Don't forget the double quotes])

#RSG Start - Actually, I think this is wrong
RTEMS_BSPOPTS_SET([RTEMS_XPPC_BASE],[*],[.])
RTEMS_BSPOPTS_HELP([RTEMS_XPPC_BASE],[Defines path to Xilinx XPS PPC libraries.])
#RSG End

RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_PRESS_KEY_FOR_RESET],
[If defined, print a message and wait until pressed before resetting
 board when application exits.])

RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_RESET_BOARD_AT_EXIT],
[If defined, reset the board when the application exits.])

BSP_BOOTCARD_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
