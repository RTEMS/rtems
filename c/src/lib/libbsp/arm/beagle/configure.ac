##
#
# @file
#
# @brief Configure script of LibBSP for the BeagleBoards.
#

AC_PREREQ([2.68])
AC_INIT([rtems-c-src-lib-libbsp-arm-beagle],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.11.1])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_BSPOPTS_SET([IS_DM3730],[beagleboard*],[1])
RTEMS_BSPOPTS_HELP([IS_DM3730],[true if SOC is DM3730])

RTEMS_BSPOPTS_SET([IS_AM335X],[beaglebone*],[1])
RTEMS_BSPOPTS_HELP([IS_AM335X],[true if SOC is AM335X])

RTEMS_BSPOPTS_SET([CONSOLE_BAUD],[*],[115200])
RTEMS_BSPOPTS_HELP([CONSOLE_BAUD],[initial baud for console UART])

RTEMS_BSPOPTS_SET([CONSOLE_POLLED],[*],[0])
RTEMS_BSPOPTS_HELP([CONSOLE_POLLED],[polled console i/o (e.g. to run testsuite)])

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)
RTEMS_BSP_LINKCMDS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
