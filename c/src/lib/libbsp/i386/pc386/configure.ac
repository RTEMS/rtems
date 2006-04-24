## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-i386-pc386],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

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
[Determines, whether RTEMS will try to use the primary IDE interface. 
 Enable it, if:
 - you have a secondary IDE interface 
 - and you have at least one disk attached to this interface  
 - and you do want to access disks attached to this interface])

## if this is an i386, does gas have good code16 support?
RTEMS_I386_GAS_CODE16
AM_CONDITIONAL(RTEMS_GAS_CODE16,[test "$RTEMS_GAS_CODE16" = "yes"])

AM_CONDITIONAL(HAS_NETWORKING,[test "$HAS_NETWORKING" = "yes"])

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

AC_SUBST(RTEMS_BSP)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
start/Makefile
startup/Makefile
timer/Makefile
3c509/Makefile
ne2000/Makefile
wd8003/Makefile
ide/Makefile
wrapup/Makefile])
AC_OUTPUT
