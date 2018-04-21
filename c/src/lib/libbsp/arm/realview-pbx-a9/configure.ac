##
#
# @file
#
# @brief Configure script of LibBSP for the RealView PBX A9 board.
#

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-realview-bpx-a9],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

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

RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[*],[100000000U])
RTEMS_BSPOPTS_HELP([BSP_ARM_A9MPCORE_PERIPHCLK],[ARM Cortex-A9 MPCore PERIPHCLK clock frequency in Hz])

RTEMS_BSPOPTS_SET([CLOCK_DRIVER_USE_FAST_IDLE],[*qemu*],[1])
RTEMS_BSPOPTS_HELP([CLOCK_DRIVER_USE_FAST_IDLE],
[This sets a mode where the time runs as fast as possible when a clock ISR
occurs while the IDLE thread is executing.  This can significantly reduce
simulation times.])

RTEMS_BSPOPTS_SET([CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR],[*qemu*],[1])
RTEMS_BSPOPTS_HELP([CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR],
[If defined, then do the clock tick processing on the boot processor on behalf
of all other processors.])


RTEMS_BSP_CLEANUP_OPTIONS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
