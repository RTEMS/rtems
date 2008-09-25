## Process this file with autoconf to produce a configure script.
## 
## $Id$
## 

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-bfin-bf537Stamp],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
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

## bsp-specific options
RTEMS_BSPOPTS_SET([CONSOLE_USE_INTERRUPTS],[*],[0])
RTEMS_BSPOPTS_HELP([CONSOLE_USE_INTERRUPTS],
[The console driver can operate in either polled or interrupt mode.])

RTEMS_BSPOPTS_SET([BF537_ON_SKYEYE],[bf537skyeye],[1])
RTEMS_BSPOPTS_SET([BF537_ON_SKYEYE],[*],[0])
RTEMS_BSPOPTS_HELP([BF537_ON_SKYEYE],
[(BSP--Skyeye)
 If defined, disable features which are not supported on Skyeye.])

BSP_BOOTCARD_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
