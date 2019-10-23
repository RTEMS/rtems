##
#
# @file
#
# @brief Configure script of LibBSP for the Xilinx Zynq platform.
#

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-xilinx-zynq-a9],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE



RTEMS_BSPOPTS_SET([BSP_START_RESET_VECTOR],[*],[])
RTEMS_BSPOPTS_HELP([BSP_START_RESET_VECTOR],[reset vector address for BSP start])

RTEMS_BSPOPTS_SET([BSP_DATA_CACHE_ENABLED],[*qemu],[])
RTEMS_BSPOPTS_SET([BSP_DATA_CACHE_ENABLED],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_DATA_CACHE_ENABLED],[enable data cache])

RTEMS_BSPOPTS_SET([BSP_INSTRUCTION_CACHE_ENABLED],[*qemu],[])
RTEMS_BSPOPTS_SET([BSP_INSTRUCTION_CACHE_ENABLED],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_INSTRUCTION_CACHE_ENABLED],[enable instruction cache])

RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[xilinx_zynq_zc702*],[333333333U])
RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[xilinx_zynq_zedboard*],[666666667U])
RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[*],[100000000U])
RTEMS_BSPOPTS_HELP([BSP_ARM_A9MPCORE_PERIPHCLK],[ARM Cortex-A9 MPCore PERIPHCLK clock frequency in Hz])

RTEMS_BSPOPTS_SET([ZYNQ_CLOCK_UART],[xilinx_zynq_zc702*],[50000000UL])
RTEMS_BSPOPTS_SET([ZYNQ_CLOCK_UART],[xilinx_zynq_zedboard*],[50000000UL])
RTEMS_BSPOPTS_SET([ZYNQ_CLOCK_UART],[*],[50000000UL])
RTEMS_BSPOPTS_HELP([ZYNQ_CLOCK_UART],[Zynq UART clock frequency in Hz])

RTEMS_BSPOPTS_SET([ZYNQ_CLOCK_CPU_1X],[xilinx_zynq_zc702*],[111111111U])
RTEMS_BSPOPTS_SET([ZYNQ_CLOCK_CPU_1X],[xilinx_zynq_zedboard*],[111111111U])
RTEMS_BSPOPTS_SET([ZYNQ_CLOCK_CPU_1X],[*],[111111111U])
RTEMS_BSPOPTS_HELP([ZYNQ_CLOCK_CPU_1X],[Zynq cpu_1x clock frequency in Hz])

USE_FAST_IDLE=0
AS_IF([test "x${RTEMS_BSP}" == xxilinx_zynq_a9_qemu], [USE_FAST_IDLE=1])

RTEMS_BSPOPTS_SET([CLOCK_DRIVER_USE_FAST_IDLE],[*qemu],[${USE_FAST_IDLE}])
RTEMS_BSPOPTS_HELP([CLOCK_DRIVER_USE_FAST_IDLE],
[This sets a mode where the time runs as fast as possible when a clock ISR
occurs while the IDLE thread is executing.  This can significantly reduce
simulation times.])

RTEMS_BSPOPTS_SET([BSP_CONSOLE_MINOR],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_CONSOLE_MINOR],[minor number of console device])

RTEMS_BSPOPTS_SET([ZYNQ_CONSOLE_USE_INTERRUPTS],[*],[1])
RTEMS_BSPOPTS_HELP([ZYNQ_CONSOLE_USE_INTERRUPTS],[use interrupt driven mode for console devices (used by default)])

#
# Zynq Memory map can be controlled from the configure command line. Use ...
#
#   ..../configure --target=arm-rtems4.11 ... BSP_ZYNQ_RAM_LENGTH=256M
#
RTEMS_BSPOPTS_SET([BSP_ZYNQ_RAM_LENGTH],[xilinx_zynq_a9_qemu],[256M])
RTEMS_BSPOPTS_SET([BSP_ZYNQ_RAM_LENGTH],[xilinx_zynq_zc702],[1024M])
RTEMS_BSPOPTS_SET([BSP_ZYNQ_RAM_LENGTH],[xilinx_zynq_zc706],[1024M])
RTEMS_BSPOPTS_SET([BSP_ZYNQ_RAM_LENGTH],[xilinx_zynq_zedboard],[512M])
RTEMS_BSPOPTS_SET([BSP_ZYNQ_RAM_LENGTH],[*],[256M])
RTEMS_BSPOPTS_HELP([BSP_ZYNQ_RAM_LENGTH],[override a BSP's default RAM length])

RTEMS_BSPOPTS_SET([BSP_ZYNQ_NOCACHE_LENGTH],[*],[1M])
RTEMS_BSPOPTS_HELP([BSP_ZYNQ_NOCACHE_LENGTH],[length of nocache RAM region])

ZYNQ_RAM_ORIGIN="0x00100000"
ZYNQ_RAM_MMU_LENGTH="16k"
ZYNQ_RAM_INT_0_ORIGIN="0x00000000"
ZYNQ_RAM_INT_0_LENGTH="64k + 64k + 64k"
ZYNQ_RAM_INT_1_ORIGIN="0xFFFF0000"
ZYNQ_RAM_INT_1_LENGTH="64k - 512"

AS_IF([test "x${RTEMS_BSP}" == xxilinx_zynq_zc706],
      [ZYNQ_RAM_ORIGIN="0x00400000"])

AC_DEFUN([ZYNQ_LINKCMD],[
AC_ARG_VAR([$1],[$2; default $3])dnl
[$1]=[$]{[$1]:-[$3]}
])

ZYNQ_LINKCMD([ZYNQ_RAM_ORIGIN],[normal RAM region origin],[${ZYNQ_RAM_ORIGIN}])
ZYNQ_LINKCMD([BSP_ZYNQ_RAM_LENGTH],[normal RAM region length],[${BSP_ZYNQ_RAM_LENGTH}])
ZYNQ_LINKCMD([ZYNQ_RAM_MMU_LENGTH],[MMU region length],[${ZYNQ_RAM_MMU_LENGTH}])
ZYNQ_LINKCMD([ZYNQ_RAM_NOCACHE_LENGTH],[length of nocache RAM region],[${BSP_ZYNQ_NOCACHE_LENGTH}])
ZYNQ_LINKCMD([ZYNQ_RAM_INT_0_ORIGIN],[internal 0 RAM region origin],[${ZYNQ_RAM_INT_0_ORIGIN}])
ZYNQ_LINKCMD([ZYNQ_RAM_INT_0_LENGTH],[internal 0 RAM region length],[${ZYNQ_RAM_INT_0_LENGTH}])
ZYNQ_LINKCMD([ZYNQ_RAM_INT_1_ORIGIN],[internal 1 RAM region origin],[${ZYNQ_RAM_INT_1_ORIGIN}])
ZYNQ_LINKCMD([ZYNQ_RAM_INT_1_LENGTH],[internal 1 RAM region length],[${ZYNQ_RAM_INT_1_LENGTH}])

RTEMS_BSP_CLEANUP_OPTIONS

AC_CONFIG_FILES([
Makefile
linkcmds:../../../../../../bsps/arm/xilinx-zynq/start/linkcmds.in])
AC_OUTPUT
