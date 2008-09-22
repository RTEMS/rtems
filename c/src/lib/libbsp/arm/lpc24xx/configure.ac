##
#
# @file
#
# @ingroup mpc55xx_config
#
# @brief Configure script of LibBSP for the LPC247X boards.
#

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libbsp-arm-lpc247x],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.9])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING 
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSP_BOOTCARD_HANDLES_RAM_ALLOCATION

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
