## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-m68k-gen68360],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")
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

BSP_BOOTCARD_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
