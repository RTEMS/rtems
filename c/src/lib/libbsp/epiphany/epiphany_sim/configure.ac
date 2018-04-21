##
#
# @file
#
# @brief Configure script of LibBSP for epiphany_sim BSP.
#

AC_PREREQ(2.69)
AC_INIT([rtems-c-src-lib-libbsp-epiphany-sim],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
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

RTEMS_BSPOPTS_SET([BSP_EPIPHANY_PERIPHCLK],[*],[100000000U])
RTEMS_BSPOPTS_HELP([BSP_EPIPHANY_PERIPHCLK],[epiphany PERIPHCLK clock
frequency in Hz])



RTEMS_BSP_CLEANUP_OPTIONS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
