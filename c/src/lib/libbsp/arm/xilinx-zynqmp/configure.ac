##
#
# @file
#
# @brief Configure script of LibBSP for the Xilinx Zynq UltraScale+ MPSoC platform.
#

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-xilinx-zynqmp-a53],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
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

RTEMS_BSPOPTS_SET([ARM_GENERIC_TIMER_USE_VIRTUAL],[*],[])
RTEMS_BSPOPTS_HELP([ARM_GENERIC_TIMER_USE_VIRTUAL],[Use virtual ARM generic timer])

RTEMS_BSPOPTS_SET([ARM_GENERIC_TIMER_FREQ],[*],[])
RTEMS_BSPOPTS_HELP([ARM_GENERIC_TIMER_FREQ],[ARM generic timer frequency in Hz])

RTEMS_BSPOPTS_SET([ZYNQ_CLOCK_UART],[xilinx_zynqmp_ultra96*],[100000000UL])
RTEMS_BSPOPTS_SET([ZYNQ_CLOCK_UART],[*],[100000000UL])
RTEMS_BSPOPTS_HELP([ZYNQ_CLOCK_UART],[Zynq UART clock frequency in Hz])

USE_FAST_IDLE=0
AS_IF([test "x${RTEMS_BSP}" == xxilinx_zynqmp_a53_qemu], [USE_FAST_IDLE=1])

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
#   ..../configure --target=arm-rtems4.11 ... BSP_ZYNQMP_RAM_LENGTH=256M
#
RTEMS_BSPOPTS_SET([BSP_ZYNQMP_RAM_LENGTH],[xilinx_zynqmp_ultra96],[2048M])
RTEMS_BSPOPTS_SET([BSP_ZYNQMP_RAM_LENGTH],[*],[256M])
RTEMS_BSPOPTS_HELP([BSP_ZYNQMP_RAM_LENGTH],[override a BSP's default RAM length])

RTEMS_BSPOPTS_SET([BSP_ZYNQMP_NOCACHE_LENGTH],[*],[1M])
RTEMS_BSPOPTS_HELP([BSP_ZYNQMP_NOCACHE_LENGTH],[length of nocache RAM region])

ZYNQMP_RAM_ORIGIN="0x00100000"
ZYNQMP_RAM_MMU_LENGTH="16k"
ZYNQMP_RAM_INT_0_ORIGIN="0x00000000"
ZYNQMP_RAM_INT_0_LENGTH="64k + 64k + 64k"
ZYNQMP_RAM_INT_1_ORIGIN="0xFFFF0000"
ZYNQMP_RAM_INT_1_LENGTH="64k - 512"

AC_DEFUN([ZYNQMP_LINKCMD],[
AC_ARG_VAR([$1],[$2; default $3])dnl
[$1]=[$]{[$1]:-[$3]}
])

ZYNQMP_LINKCMD([ZYNQMP_RAM_ORIGIN],[normal RAM region origin],[${ZYNQMP_RAM_ORIGIN}])
ZYNQMP_LINKCMD([ZYNQMP_RAM_LENGTH],[normal RAM region length],[${BSP_ZYNQMP_RAM_LENGTH}])
ZYNQMP_LINKCMD([ZYNQMP_RAM_MMU_LENGTH],[MMU region length],[${ZYNQMP_RAM_MMU_LENGTH}])
ZYNQMP_LINKCMD([ZYNQMP_RAM_NOCACHE_LENGTH],[length of nocache RAM region],[${BSP_ZYNQMP_NOCACHE_LENGTH}])
ZYNQMP_LINKCMD([ZYNQMP_RAM_INT_0_ORIGIN],[internal 0 RAM region origin],[${ZYNQMP_RAM_INT_0_ORIGIN}])
ZYNQMP_LINKCMD([ZYNQMP_RAM_INT_0_LENGTH],[internal 0 RAM region length],[${ZYNQMP_RAM_INT_0_LENGTH}])
ZYNQMP_LINKCMD([ZYNQMP_RAM_INT_1_ORIGIN],[internal 1 RAM region origin],[${ZYNQMP_RAM_INT_1_ORIGIN}])
ZYNQMP_LINKCMD([ZYNQMP_RAM_INT_1_LENGTH],[internal 1 RAM region length],[${ZYNQMP_RAM_INT_1_LENGTH}])

RTEMS_BSP_CLEANUP_OPTIONS

AC_CONFIG_FILES([
Makefile
linkcmds:../../../../../../bsps/arm/xilinx-zynqmp/start/linkcmds.in])
AC_OUTPUT
