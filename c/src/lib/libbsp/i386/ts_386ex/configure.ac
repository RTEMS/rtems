## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-i386-ts_386ex,$RTEMS_VERSION,no)
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_NETWORKING

## if this is an i386, does gas have good code16 support?
RTEMS_I386_GAS_CODE16
AM_CONDITIONAL(RTEMS_GAS_CODE16,test "$RTEMS_GAS_CODE16" = "yes")

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

# bsp-specific options
RTEMS_BSPOPTS_SET([LOAD_RTC_AT_START],[*],[1])
RTEMS_BSPOPTS_HELP([LOAD_RTC_AT_START],
[Initialize RTC and load into system clock at startup. This will break
some tests.])

RTEMS_BSPOPTS_SET([BSP_DEBUG],[*],[])
RTEMS_BSPOPTS_HELP([BSP_DEBUG],
[Enables some debugging in the BSP package.])

RTEMS_BSPOPTS_SET([UARTDEBUG],[*],[])
RTEMS_BSPOPTS_HELP([UARTDEBUG],
[Enables some debugging in the BSP's uart.])


# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
start/Makefile
startup/Makefile
timer/Makefile
network/Makefile
timer/Makefile
wrapup/Makefile])
AC_OUTPUT
