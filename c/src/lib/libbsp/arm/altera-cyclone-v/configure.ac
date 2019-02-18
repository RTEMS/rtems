##
#
# @file
#
# @brief Configure script of LibBSP for the Altera Cyclone-V platform.
#

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-xilinx-zynq-a9],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE



RTEMS_BSPOPTS_SET([BSP_START_RESET_VECTOR],[*],[])
RTEMS_BSPOPTS_HELP([BSP_START_RESET_VECTOR],[reset vector address for BSP start])

RTEMS_BSPOPTS_SET([BSP_DATA_CACHE_ENABLED],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_DATA_CACHE_ENABLED],[enable data cache])

RTEMS_BSPOPTS_SET([BSP_INSTRUCTION_CACHE_ENABLED],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_INSTRUCTION_CACHE_ENABLED],[enable instruction cache])

RTEMS_BSPOPTS_SET([BSP_FDT_IS_SUPPORTED],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_FDT_IS_SUPPORTED],[define if FDT is supported])

RTEMS_BSPOPTS_SET([BSP_START_COPY_FDT_FROM_U_BOOT],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_START_COPY_FDT_FROM_U_BOOT],[copy the U-Boot provided FDT to an internal storage])

RTEMS_BSPOPTS_SET([BSP_FDT_BLOB_SIZE_MAX],[*],[262144])
RTEMS_BSPOPTS_HELP([BSP_FDT_BLOB_SIZE_MAX],[maximum size of the FDT blob in bytes])

RTEMS_BSPOPTS_SET([BSP_FDT_BLOB_READ_ONLY],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_FDT_BLOB_READ_ONLY],[place the FDT blob into the read-only data area])

RTEMS_BSPOPTS_SET([BSP_FDT_BLOB_COPY_TO_READ_ONLY_LOAD_AREA],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_FDT_BLOB_COPY_TO_READ_ONLY_LOAD_AREA],[copy the FDT blob into the read-only load area via bsp_fdt_copy()])

RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[*],[])
RTEMS_BSPOPTS_HELP([BSP_ARM_A9MPCORE_PERIPHCLK],[define to set ARM Cortex-A9 MPCore PERIPHCLK clock frequency in Hz, otherwise alt_clk_freq_get() is used])

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


RTEMS_BSP_CLEANUP_OPTIONS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
