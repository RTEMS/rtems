## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-sparc-erc32],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
RTEMS_CHECK_SMP

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")
AM_CONDITIONAL(HAS_SMP,[test "$rtems_cv_HAS_SMP" = "yes"])

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

RTEMS_BSPOPTS_SET([ENABLE_SIS_QUIRKS],[sis],[1])
RTEMS_BSPOPTS_SET([ENABLE_SIS_QUIRKS],[*],[0])
RTEMS_BSPOPTS_HELP([ENABLE_SIS_QUIRKS],
[If defined, then the SIS simulator specific code in the
 BSP will be enabled.  In particular, SIS requires special
 initialization not used on real ERC32 hardware.])

RTEMS_BSP_CLEANUP_OPTIONS(0,0)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
