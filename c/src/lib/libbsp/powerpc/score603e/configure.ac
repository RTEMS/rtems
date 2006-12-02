## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-ep1a],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

## FIXME: This should be a 1 out of 3 selection
## and is somehow coupled to USE_DINK (cf. include/gen2.h)
RTEMS_BSPOPTS_SET([SCORE603E_USE_SDS],[*],[0])
RTEMS_BSPOPTS_HELP([SCORE603E_USE_SDS],
[use SDS ROM monitor])

RTEMS_BSPOPTS_SET([SCORE603E_OPEN_FIRMWARE],[*],[0])
RTEMS_BSPOPTS_HELP([SCORE603E_OPEN_FIRMWARE],
[use Open Firmware ROM monitor])

RTEMS_BSPOPTS_SET([SCORE603E_USE_NONE],[*],[0])
RTEMS_BSPOPTS_HELP([SCORE603E_USE_NONE],
[use no ROM monitor])

RTEMS_BSPOPTS_SET([SCORE603E_USE_DINK],[*],[1])
RTEMS_BSPOPTS_HELP([SCORE603E_USE_DINK],
[FIXME: Missing explanation.])

RTEMS_BSPOPTS_SET([CONSOLE_USE_INTERRUPTS],[*],[0])
RTEMS_BSPOPTS_HELP([CONSOLE_USE_INTERRUPTS],
[whether using console interrupts])

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
