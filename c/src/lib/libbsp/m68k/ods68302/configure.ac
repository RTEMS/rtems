## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-m68k-ods68302],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

AC_ARG_ENABLE(ods68302-debug,
[AS_HELP_STRING(--enable-ods68302-debug,whether to support bsp debugging)],
[case $enable_ods68302_debug in
 yes ) ODS68302_DEBUG=yes;;
 * )   ODS68302_DEBUG=no;;
 esac],
[ODS68302_DEBUG=no])

AM_CONDITIONAL(ODS68302_DEBUG,[test "$ODS68302_DEBUG" = "yes"])

# FIXME: Something seem to be very broken inside of this BSP
#        It plays tricks with VARIANT
#        For now, deactivate it.
# BSP_VARIANT=${BSP_VARIANT-bare}
# AC_DEFINE_UNQUOTED([VARIANT],[${BSP_VARIANT}],[missing explanation])

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
