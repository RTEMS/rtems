##
#
# @file
#
# @brief Configure script of LibBSP for the Altera Cyclone-V platform.
#

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-xilinx-zynq-a9],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([BSP_START_RESET_VECTOR],[*],[])
RTEMS_BSPOPTS_HELP([BSP_START_RESET_VECTOR],[reset vector address for BSP start])

RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[altcycv_devkit*],[200000000U])
RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[*],[100000000U])
RTEMS_BSPOPTS_HELP([BSP_ARM_A9MPCORE_PERIPHCLK],[ARM Cortex-A9 MPCore PERIPHCLK clock frequency in Hz])

RTEMS_BSPOPTS_SET([CLOCK_DRIVER_USE_FAST_IDLE],[*qemu],[1])
RTEMS_BSPOPTS_HELP([CLOCK_DRIVER_USE_FAST_IDLE],
[This sets a mode where the time runs as fast as possible when a clock ISR
occurs while the IDLE thread is executing.  This can significantly reduce
simulation times.])

RTEMS_BSPOPTS_SET([BSP_CONSOLE_MINOR],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_CONSOLE_MINOR],[minor number of console device])

RTEMS_BSPOPTS_SET([CYCLONE_V_CONFIG_CONSOLE],[*],[0])
RTEMS_BSPOPTS_HELP([CYCLONE_V_CONFIG_CONSOLE],[configuration for console (UART 0)])

RTEMS_BSPOPTS_SET([CYCLONE_V_CONFIG_UART_1],[*],[0])
RTEMS_BSPOPTS_HELP([CYCLONE_V_CONFIG_UART_1],[configuration for UART 1])

RTEMS_BSPOPTS_SET([CYCLONE_V_UART_BAUD],[*],[115200U])
RTEMS_BSPOPTS_HELP([CYCLONE_V_UART_BAUD],[baud for UARTs])

RTEMS_BSPOPTS_SET([BSP_USE_UART_INTERRUPTS],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_USE_UART_INTERRUPTS],[enable usage of interrupts for the UART modules])

RTEMS_BSPOPTS_SET([CYCLONE_V_NO_I2C],[*],[1])
RTEMS_BSPOPTS_HELP([CYCLONE_V_NO_I2C],
[Number of configured I2C buses. Note that each bus has to be configured in an
apropriate i2cdrv_config array.])

RTEMS_BSPOPTS_SET([CYCLONE_V_I2C0_SPEED],[*],[100000])
RTEMS_BSPOPTS_HELP([CYCLONE_V_I2C0_SPEED],[speed for I2C0 in HZ])

RTEMS_CHECK_SMP
AM_CONDITIONAL(HAS_SMP,[test "$rtems_cv_HAS_SMP" = "yes"])

RTEMS_BSP_CLEANUP_OPTIONS(0, 1)
RTEMS_BSP_LINKCMDS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
