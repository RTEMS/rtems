#
# @file
#
# @brief Configure script of LibBSP for generic_or1k BSP.
#

AC_PREREQ(2.69)
AC_INIT([rtems-c-src-lib-libbsp-or1k-generic],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_BSPOPTS_SET([BSP_START_RESET_VECTOR],[*],[])
RTEMS_BSPOPTS_HELP([BSP_START_RESET_VECTOR],[reset vector address for BSP start])

RTEMS_BSPOPTS_SET([BSP_GENERIC_OR1K_PERIPHCLK],[*],[50000000U])
RTEMS_BSPOPTS_HELP([BSP_GENERIC_OR1K_PERIPHCLK],[or1k PERIPHCLK clock frequency in Hz])


RTEMS_BSP_CLEANUP_OPTIONS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
