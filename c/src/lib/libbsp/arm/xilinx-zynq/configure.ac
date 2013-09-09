##
#
# @file
#
# @brief Configure script of LibBSP for the Xilinx Zynq platform.
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

RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[xilinx_zynq_zc702*],[666666667U])
RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[xilinx_zynq_zedboard*],[666666667U])
RTEMS_BSPOPTS_SET([BSP_ARM_A9MPCORE_PERIPHCLK],[*],[100000000U])
RTEMS_BSPOPTS_HELP([BSP_ARM_A9MPCORE_PERIPHCLK],[ARM Cortex-A9 MPCore PERIPHCLK clock frequency in Hz])

RTEMS_BSPOPTS_SET([CLOCK_DRIVER_USE_FAST_IDLE],[*qemu],[1])
RTEMS_BSPOPTS_HELP([CLOCK_DRIVER_USE_FAST_IDLE],
[This sets a mode where the time runs as fast as possible when a clock ISR
occurs while the IDLE thread is executing.  This can significantly reduce
simulation times.])

RTEMS_BSPOPTS_SET([BSP_CONSOLE_MINOR],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_CONSOLE_MINOR],[minor number of console device])

RTEMS_CHECK_SMP
AM_CONDITIONAL(HAS_SMP,[test "$rtems_cv_HAS_SMP" = "yes"])

RTEMS_BSP_CLEANUP_OPTIONS(0, 1)
RTEMS_BSP_LINKCMDS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
