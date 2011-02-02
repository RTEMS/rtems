##
#
# @file
#
# @ingroup mpc55xx_config
#
# @brief Configure script of LibBSP for the MPC55xx evaluation boards.
#

AC_PREREQ([2.68])
AC_INIT([rtems-c-src-lib-libbsp-powerpc-mpc55xxevb],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.11.1])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([DATA_CACHE_ENABLE],[gwlcfm],[0])
RTEMS_BSPOPTS_SET([DATA_CACHE_ENABLE],[mpc5566evb],[1])
RTEMS_BSPOPTS_SET([DATA_CACHE_ENABLE],[*],[1])
RTEMS_BSPOPTS_HELP([DATA_CACHE_ENABLE],
[If defined, the data cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([INSTRUCTION_CACHE_ENABLE],[gwlcfm],[0])
RTEMS_BSPOPTS_SET([INSTRUCTION_CACHE_ENABLE],[mpc5566evb],[1])
RTEMS_BSPOPTS_SET([INSTRUCTION_CACHE_ENABLE],[*],[1])
RTEMS_BSPOPTS_HELP([INSTRUCTION_CACHE_ENABLE],
[If defined, the instruction cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([UARTS_USE_TERMIOS],[*],[0])
RTEMS_BSPOPTS_HELP([UARTS_USE_TERMIOS],
[Define to 1 if you want termios support for every port.
 Termios support is independent of the choice of UART I/O mode.])

RTEMS_BSPOPTS_SET([CONSOLE_MINOR],[gwlcfm],[MPC55XX_ESCI_A_MINOR])
RTEMS_BSPOPTS_SET([CONSOLE_MINOR],[*]     ,[MPC55XX_ESCI_A_MINOR])
RTEMS_BSPOPTS_HELP([CONSOLE_MINOR],
[Must be defined to be one of MPC55XX_ESCI_A_MINOR or MPC55XX_ESCI_B_MINOR.  Determines which
 device will be registered as /dev/console.])

RTEMS_BSPOPTS_SET([UARTS_IO_MODE],[gwlcfm],[1])
RTEMS_BSPOPTS_SET([UARTS_IO_MODE],[*]     ,[0])
RTEMS_BSPOPTS_HELP([UARTS_IO_MODE],
[Define to 1 if you want interrupt-driven I/O for the SCI ports.])

RTEMS_BSPOPTS_SET([PRINTK_MINOR],[gwlcfm],[MPC55XX_ESCI_A_MINOR])
RTEMS_BSPOPTS_SET([PRINTK_MINOR],[*]     ,[MPC55XX_ESCI_B_MINOR])
RTEMS_BSPOPTS_HELP([PRINTK_MINOR],
[Must be defined to be one of MPC55XX_ESCI_A_MINOR or MPC55XX_ESCI_B_MINOR.  Determines which
 device is used for output by printk().  The printk port always uses polled
 I/O.  Don't open the printk port from RTEMS unless also using polled I/O
 for the SCI ports.])

RTEMS_BSPOPTS_SET([MPC55XX_FMPLL_REF_CLOCK],[gwlcfm],[40000000])
RTEMS_BSPOPTS_SET([MPC55XX_FMPLL_REF_CLOCK],[*]     ,[8000000])
RTEMS_BSPOPTS_HELP([MPC55XX_FMPLL_REF_CLOCK],
[Must be defined to be the external reference clock (in Hz) 
 for clock generation])

RTEMS_BSPOPTS_SET([MPC55XX_FMPLL_CLK_OUT],[gwlcfm],[66000000])
RTEMS_BSPOPTS_SET([MPC55XX_FMPLL_CLK_OUT],[*]     ,[128000000])
RTEMS_BSPOPTS_HELP([MPC55XX_FMPLL_CLK_OUT],
[Must be defined to be the PLL output clock (in Hz) for clock generation])

RTEMS_BSPOPTS_SET([MPC55XX_FMPLL_PREDIV],[gwlcfm],[10])
RTEMS_BSPOPTS_SET([MPC55XX_FMPLL_PREDIV],[*]     ,[1])
RTEMS_BSPOPTS_HELP([MPC55XX_FMPLL_PREDIV],
[Must be defined to be the PLL predivider factor for clock generation])

RTEMS_BSPOPTS_SET([MPC55XX_FMPLL_MFD],[gwlcfm],[99])
RTEMS_BSPOPTS_SET([MPC55XX_FMPLL_MFD],[*]     ,[12])
RTEMS_BSPOPTS_HELP([MPC55XX_FMPLL_MFD],
[Must be defined to be the PLL multiplication factor for clock generation])

RTEMS_BSPOPTS_SET([MPC55XX_EMIOS_PRESCALER],[gwlcfm],[66])
RTEMS_BSPOPTS_SET([MPC55XX_EMIOS_PRESCALER],[*]     ,[0])
RTEMS_BSPOPTS_HELP([MPC55XX_EMIOS_PRESCALER],
[Must be defined to set the EMIOS prescaler])

RTEMS_BSPOPTS_SET([MPC55XX_CHIP_TYPE],[mpc5566evb],[5566])
RTEMS_BSPOPTS_SET([MPC55XX_CHIP_TYPE],[gwlcfm]    ,[5516])
RTEMS_BSPOPTS_SET([MPC55XX_CHIP_TYPE],[*]         ,[5554])
RTEMS_BSPOPTS_HELP([MPC55XX_CHIP_TYPE],
[specifies the chip type in use (e.g. 5554 for MPC5554)])

RTEMS_BSPOPTS_SET([BOARD_GWLCFM],[gwlcfm],[1])
RTEMS_BSPOPTS_HELP([BOARD_GWLCFM],
[If defined, use custom settings of for the GW_LCFM board])

AC_CONFIG_FILES([Makefile
include/bspopts.h])

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
