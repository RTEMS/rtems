##
#
# @file
#
# @brief Configure script of LibBSP for epiphany_sim BSP.
#

AC_PREREQ(2.69)
AC_INIT([rtems-c-src-lib-libbsp-epiphany-sim],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign subdir-objects 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_BSPOPTS_SET([BSP_START_RESET_VECTOR],[*],[])
RTEMS_BSPOPTS_HELP([BSP_START_RESET_VECTOR],[reset vector address for BSP
start])

RTEMS_BSPOPTS_SET([BSP_EPIPHANY_PERIPHCLK],[*],[100000000U])
RTEMS_BSPOPTS_HELP([BSP_EPIPHANY_PERIPHCLK],[epiphany PERIPHCLK clock
frequency in Hz])

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_SMP
AM_CONDITIONAL(HAS_SMP,[test "$rtems_cv_HAS_SMP" = "yes"])

RTEMS_BSP_CLEANUP_OPTIONS(0, 1)
RTEMS_BSP_LINKCMDS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
