dnl Process this file with autoconf to produce a configure script.
dnl
dnl $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-powerpc-gen405,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_ENABLE_NETWORKING

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)
RTEMS_CHECK_NETWORKING
RTEMS_CANONICAL_HOST

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([PPC_USE_SPRG],[*],[1])
RTEMS_BSPOPTS_HELP([PPC_USE_SPRG],
[If defined, then the PowerPC specific code in RTEMS will use some
 of the special purpose registers to slightly optimize interrupt  
 response time.  The use of these registers can conflict with     
 other tools like debuggers.])

RTEMS_BSPOPTS_SET([PPC_VECTOR_FILE_BASE],[*],[0x0100])
RTEMS_BSPOPTS_HELP([PPC_VECTOR_FILE_BASE],
[This defines the base address of the exception table.
 NOTE: Vectors are actually at 0xFFF00000 but file starts at offset.])

AM_CONFIG_HEADER(include/bspopts.h)
RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
dlentry/Makefile
include/Makefile
startup/Makefile
wrapup/Makefile])

RTEMS_PPC_EXCEPTIONS([old])

AC_OUTPUT
