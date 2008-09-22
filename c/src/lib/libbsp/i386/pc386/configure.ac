## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-i386-pc386],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_CHECK_TOOL(OBJCOPY,objcopy,:)
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING

RTEMS_BSPOPTS_SET([USE_COM1_AS_CONSOLE],[*],[0])
RTEMS_BSPOPTS_HELP([USE_COM1_AS_CONSOLE],
[Determines, whether the console will be associated with the standard
 VGA display or with the COM1 serial port.  Currently only the VGA
 display and COM1 support printk.])

RTEMS_BSPOPTS_SET([IDE_USE_PRIMARY_INTERFACE],[*],[1])
RTEMS_BSPOPTS_HELP([IDE_USE_PRIMARY_INTERFACE],
[Determines, whether RTEMS will try to use the primary IDE interface. 
 Disable it, if:
 - you have no primary IDE interface 
 - or you have no disk attached to this interface  
 - or you do not want to access disks attached to this interface])

RTEMS_BSPOPTS_SET([IDE_USE_SECONDARY_INTERFACE],[*],[0])
RTEMS_BSPOPTS_HELP([IDE_USE_SECONDARY_INTERFACE],
[Determines, whether RTEMS will try to use the secondary IDE interface. 
 Enable it, if:
 - you have a secondary IDE interface 
 - and you have at least one disk attached to this interface  
 - and you do want to access disks attached to this interface])

RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_PRESS_KEY_FOR_RESET],
[Determines, whether this BSP will wait for a key to be pressed 
 before rebooting the PC.  This is useful for unattended PC deployments
])

## if this is an i386, does gas have good code16 support?
RTEMS_I386_GAS_CODE16
AM_CONDITIONAL(RTEMS_GAS_CODE16,[test "$RTEMS_GAS_CODE16" = "yes"])

AM_CONDITIONAL(HAS_NETWORKING,[test "$HAS_NETWORKING" = "yes"])

AC_SUBST([RTEMS_ROOT],[${rtems_updir}'$(top_builddir)'])

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

AC_SUBST(RTEMS_BSP)

RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_PRESS_KEY_FOR_RESET],
[If defined, print a message and wait until pressed before resetting
 board when application exits.])

RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_RESET_BOARD_AT_EXIT],
[If defined, reset the board when the application exits.])

RTEMS_BSP_BOOTCARD_HANDLES_RAM_ALLOCATION

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
