## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libbsp-sh-gensh1],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.8])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

## bsp-specific options
RTEMS_BSPOPTS_SET([CPU_CLOCK_RATE_HZ],[*],[20000000])
RTEMS_BSPOPTS_HELP([CPU_CLOCK_RATE_HZ],[cpu clock rate in HZ])

## Used in scitab/Makefile.am
AC_SUBST(CPU_CLOCK_RATE_HZ)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
console/Makefile
scitab/Makefile
startup/Makefile
wrapup/Makefile])
AC_OUTPUT
