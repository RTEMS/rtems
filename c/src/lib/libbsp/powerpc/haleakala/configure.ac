dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-powerpc-haleakala],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE



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

AC_DEFINE(ppc405, 1, [PowerPC model option])

RTEMS_BSP_CLEANUP_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
