## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-powerpc-mbx5xx],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([WATCHDOG_TIMEOUT],[*],[0xFFFF])
RTEMS_BSPOPTS_HELP([WATCHDOG_TIMEOUT],
[Define to the desired timeout (in steps of 1/20 msec) to enable the
 watchdog.  Default is to disable the watchdog entirely.])

RTEMS_BSPOPTS_SET([UARTS_USE_TERMIOS],[*],[0])
RTEMS_BSPOPTS_HELP([UARTS_USE_TERMIOS],
[Define to 1 if you want termios support for every port.
 Termios support is independent of the choice of UART I/O mode.])

RTEMS_BSPOPTS_SET([CONSOLE_MINOR],[*],[SCI2_MINOR])
RTEMS_BSPOPTS_HELP([CONSOLE_MINOR],
[Must be defined to be one of SCI1_MINOR or SCI2_MINOR.  Determines which
 device will be registered as /dev/console.])

RTEMS_BSPOPTS_SET([UARTS_IO_MODE],[*],[0])
RTEMS_BSPOPTS_HELP([UARTS_IO_MODE],
[Define to 1 if you want interrupt-driven I/O for the SCI ports.])

RTEMS_BSPOPTS_SET([PRINTK_MINOR],[*],[SCI2_MINOR])
RTEMS_BSPOPTS_HELP([PRINTK_MINOR],
[Must be defined to be one of SCI1_MINOR or SCI2_MINOR.  Determines which
 device is used for output by printk().  The printk port always uses polled
 I/O.  Don't open the printk port from RTEMS unless also using polled I/O
 for the SCI ports.])

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)

# Explicitly list a Makefile here
AC_CONFIG_FILES([Makefile])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
