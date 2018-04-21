##
#
# @file
#
# @brief Configure script of LibBSP for riscv_generic BSP.
#

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-riscv_generic],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_BSPOPTS_SET([BSP_START_RESET_VECTOR],[*],[])
RTEMS_BSPOPTS_HELP([BSP_START_RESET_VECTOR],[reset vector address for BSP
start])

RTEMS_BSPOPTS_SET([BSP_RISCV32_PERIPHCLK],[*],[100000000U])
RTEMS_BSPOPTS_HELP([BSP_RISCV32_PERIPHCLK],[riscv PERIPHCLK clock
frequency in Hz])



RTEMS_BSP_CLEANUP_OPTIONS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
