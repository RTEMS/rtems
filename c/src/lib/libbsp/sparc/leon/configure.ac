## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-lib-libbsp-sparc-leon],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.6])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

## bsp-specific options
RTEMS_BSPOPTS_SET([CONSOLE_USE_INTERRUPTS],[*],[0])
RTEMS_BSPOPTS_HELP([CONSOLE_USE_INTERRUPTS],
[The erc32 console driver can operate in either polled or interrupt mode.
Under the simulator (especially when FAST_UART is defined), polled seems to operate 
better.  It is common for a task to print a line (like the end of test message) and
then exit.  In this case, the program returns control to the simulator command line
before the program has even queued the output to the uart.  Thus sis has no chance
of getting the data out.])

RTEMS_BSPOPTS_SET([SIMSPARC_FAST_IDLE],[*],[])
RTEMS_BSPOPTS_HELP([SIMSPARC_FAST_IDLE],
[If defined, speed up the clock ticks while the idle task is running so
 time spent in the idle task is minimized.  This significantly reduces
 the wall time required to execute the RTEMS test suites.])

RTEMS_BSPOPTS_SET([NO_TABLE_MOVE],[*],[1])
RTEMS_BSPOPTS_HELP([NO_TABLE_MOVE],
[do not have a second trap table -- use the BSP's.])

RTEMS_BSPOPTS_SET([FPU_REVB],[*],[])
RTEMS_BSPOPTS_HELP([FPU_REVB],
[If defined, enables work-around for bug 3.14 in FPU rev.B or rev.C])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
gnatsupp/Makefile
include/Makefile
start/Makefile
startup/Makefile
timer/Makefile
wrapup/Makefile])
AC_OUTPUT
