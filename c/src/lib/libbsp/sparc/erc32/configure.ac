## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-sparc-erc32],
[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE




## bsp-specific options
RTEMS_BSPOPTS_SET([CONSOLE_USE_INTERRUPTS],[*],[0])
RTEMS_BSPOPTS_HELP([CONSOLE_USE_INTERRUPTS],
[The erc32 console driver can operate in either polled or interrupt mode.
Under the simulator (especially when FAST_UART is defined), polled seems
to operate better. It is common for a task to print a line (like the end
of test message) and then exit.  In this case, the program returns control
to the simulator command line before the program has even queued the output
to the uart.  Thus sis has no chance of getting the data out.])

RTEMS_BSPOPTS_SET([BSP_POWER_DOWN_AT_FATAL_HALT],[*],[])
RTEMS_BSPOPTS_HELP([BSP_POWER_DOWN_AT_FATAL_HALT],
[If defined, CPU is spinning on fatal exit. Otherwise generate system
 error which will hand over to debugger, simulator, etc.])

RTEMS_BSP_CLEANUP_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
