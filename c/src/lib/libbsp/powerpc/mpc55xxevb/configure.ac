##
#
# @file
#
# @ingroup mpc55xx_config
#
# @brief Configure script of LibBSP for the MPC55xx evaluation boards.
#

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-mpc55xxevb],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
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

RTEMS_BSPOPTS_SET([UARTS_USE_TERMIOS],[*],[0])
RTEMS_BSPOPTS_HELP([UARTS_USE_TERMIOS],
[Define to 1 if you want termios support for every port.
 Termios support is independent of the choice of UART I/O mode.])

RTEMS_BSPOPTS_SET([CONSOLE_MINOR],[*],[MPC55XX_ESCI_A_MINOR])
RTEMS_BSPOPTS_HELP([CONSOLE_MINOR],
[Must be defined to be one of MPC55XX_ESCI_A_MINOR or MPC55XX_ESCI_B_MINOR.  Determines which
 device will be registered as /dev/console.])

RTEMS_BSPOPTS_SET([UARTS_IO_MODE],[*],[0])
RTEMS_BSPOPTS_HELP([UARTS_IO_MODE],
[Define to 1 if you want interrupt-driven I/O for the SCI ports.])

RTEMS_BSPOPTS_SET([PRINTK_MINOR],[*],[MPC55XX_ESCI_B_MINOR])
RTEMS_BSPOPTS_HELP([PRINTK_MINOR],
[Must be defined to be one of MPC55XX_ESCI_A_MINOR or MPC55XX_ESCI_B_MINOR.  Determines which
 device is used for output by printk().  The printk port always uses polled
 I/O.  Don't open the printk port from RTEMS unless also using polled I/O
 for the SCI ports.])

AC_CONFIG_FILES([Makefile
include/bspopts.h])

RTEMS_BSP_BOOTCARD_HANDLES_RAM_ALLOCATION

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
