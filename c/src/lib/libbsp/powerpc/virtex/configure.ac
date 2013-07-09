dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-powerpc-virtex],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
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

RTEMS_BSPOPTS_SET([BSP_START_STACK_SIZE],[*],[4096])
RTEMS_BSPOPTS_HELP([BSP_START_STACK_SIZE],[size of low-level start stack])

RTEMS_BSPOPTS_SET([RTEMS_XPARAMETERS_H],[*],[\<xparameters_dflt.h\>])
RTEMS_BSPOPTS_HELP([RTEMS_XPARAMETERS_H],
[This defines the location of the hardware specific "xparameters.h" file.
 in the file system. Specify an absolute path. Do not forget the double quotes])

RTEMS_BSPOPTS_SET([VIRTEX_CONSOLE_USE_INTERRUPTS],[*],[1])
RTEMS_BSPOPTS_HELP([VIRTEX_CONSOLE_USE_INTERRUPTS],[if defined use interrupt driven Termios mode])

AC_DEFUN([VIRTEX_REGION],[
AC_ARG_VAR([$1],[$2; default $3])dnl
[$1]=[$]{[$1]:-[$3]}
])

VIRTEX_REGION([VIRTEX_RAM_ORIGIN],[normal RAM region origin],[0x0])
VIRTEX_REGION([VIRTEX_RAM_LENGTH],[normal RAM region length],[0x7ffffec])
VIRTEX_REGION([VIRTEX_FAST_RAM_ORIGIN],[fast RAM region origin],[0x0])
VIRTEX_REGION([VIRTEX_FAST_RAM_LENGTH],[fast RAM region length],[0x0])
VIRTEX_REGION([VIRTEX_RESET_ORIGIN],[reset region origin],[0x7ffffec])
VIRTEX_REGION([VIRTEX_RESET_LENGTH],[reset region length],[0x14])

RTEMS_BSP_CLEANUP_OPTIONS(0, 1)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([
Makefile
startup/linkcmds
])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
