## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-arm-armulator],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

# From newlib
# Select which debug protocol is being used.
# ARM_RDP_MONITOR selects the Demon monitor.
# ARM_RDI_MONITOR selects the Angel monitor.
# If neither are defined, then hard coded defaults will be used
# to create the program's environment.

RTEMS_BSPOPTS_SET([ARM_RDI_MONITOR],[*],[1])
RTEMS_BSPOPTS_HELP([ARM_RDI_MONITOR],
[If defined enable Angel monitor support])

RTEMS_BSPOPTS_SET([ARM_RDP_MONITOR],[*],[])
RTEMS_BSPOPTS_HELP([ARM_RDP_MONITOR],
[If defined enable Demon monitor support])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
start/Makefile
startup/Makefile
timer/Makefile
wrapup/Makefile])
AC_OUTPUT
