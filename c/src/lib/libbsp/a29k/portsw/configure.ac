## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-a29k-portsw],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

/* only one of the "INPUT"-defines can be defined */
RTEMS_BSPOPTS_SET([SERIAL_INPUT],[*],[1])
RTEMS_BSPOPTS_HELP([SERIAL_INPUT],[use serial input])

RTEMS_BSPOPTS_SET([HIF_INPUT],[*],[])
RTEMS_BSPOPTS_HELP([HIF_INPUT],[use HIF input])

/* both of the following can be defined */
RTEMS_BSPOPTS_SET([SERIAL_OUTPUT],[*],[1])
RTEMS_BSPOPTS_HELP([SERIAL_OUTPUT],
[use serial console output])

RTEMS_BSPOPTS_SET([HIF_OUTPUT],[*],[])
RTEMS_BSPOPTS_HELP([HIF_OUTPUT],
[use HIF console output])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
console/Makefile
include/Makefile
start/Makefile
startup/Makefile
wrapup/Makefile])
AC_OUTPUT
