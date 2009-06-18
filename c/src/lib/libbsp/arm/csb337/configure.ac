## Process this file with autoconf to produce a configure script.
## 
## configure.ac,v 1.5 2003/03/11 09:39:07 ralf Exp

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-arm-csb337],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING 
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([csb637],[csb637],[1])
RTEMS_BSPOPTS_SET([csb637],[*],[0])
RTEMS_BSPOPTS_HELP([csb637],
[If defined, this indicates that the BSP is being built for the
 csb637 variant.])

RTEMS_BSPOPTS_SET([ENABLE_LCD],[*],[0])
RTEMS_BSPOPTS_HELP([ENABLE_LCD],
[If defined, enable use of the SED1356 controller and LCD.])
AM_CONDITIONAL(ENABLE_LCD,test "$ENABLE_LCD" = "1")

RTEMS_BSPOPTS_SET([ENABLE_UMON_CONSOLE],[*],[1])
RTEMS_BSPOPTS_HELP([ENABLE_UMON_CONSOLE],
[If defined, enable use of the MicroMonitor console device.])
AM_CONDITIONAL(ENABLE_LCD,test "$ENABLE_UMON_CONSOLE" = "1")

RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_PRESS_KEY_FOR_RESET],
[If defined, print a message and wait until pressed before resetting
 board when application exits.])

RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_RESET_BOARD_AT_EXIT],
[If defined, reset the board when the application exits.])

BSP_BOOTCARD_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
