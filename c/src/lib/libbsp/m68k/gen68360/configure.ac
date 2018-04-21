## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-m68k-gen68360],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE



AM_CONDITIONAL(HAS_FPSP,[test "$RTEMS_CPU_MODEL" = "m68040"])

RTEMS_BSPOPTS_SET([PGH360],[pgh360],[1])
RTEMS_BSPOPTS_HELP([PGH360],
[If defined, use custom settings for the pgh360 BSP.])

RTEMS_BSPOPTS_SET([GEN68360],[gen68360],[1])
RTEMS_BSPOPTS_HELP([GEN68360],
[If defined, use custom settings for the gen68360 BSP.])

RTEMS_BSPOPTS_SET([GEN68360_040],[gen68360_040],[1])
RTEMS_BSPOPTS_HELP([GEN68360_040],
[If defined, use custom settings for the gen68360_040 BSP.])

RTEMS_BSP_CLEANUP_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
