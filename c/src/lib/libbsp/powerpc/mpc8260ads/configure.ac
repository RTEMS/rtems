## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-powerpc-mpc8260ads],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_BSPOPTS_SET_DATA_CACHE_ENABLED([*],[])
RTEMS_BSPOPTS_HELP_DATA_CACHE_ENABLED

RTEMS_BSPOPTS_SET_INSTRUCTION_CACHE_ENABLED([*],[])
RTEMS_BSPOPTS_HELP_INSTRUCTION_CACHE_ENABLED

RTEMS_BSPOPTS_SET([UARTS_USE_TERMIOS],[*],[0])
RTEMS_BSPOPTS_HELP([UARTS_USE_TERMIOS],
[Define to 1 if you want termios support for every port.
 Termios support is independent of the choice of UART I/O mode.])

RTEMS_BSPOPTS_SET([CONSOLE_MINOR],[*],[SCC2_MINOR])
RTEMS_BSPOPTS_HELP([CONSOLE_MINOR],
[(BSP--console driver) Must be defined to be one of SMC1_MINOR,
 SMC2_MINOR, SCC2_MINOR, SCC3_MINOR, or SCC4_MINOR.
 Determines which device will be registered as /dev/console.])

RTEMS_BSPOPTS_SET([UARTS_IO_MODE],[*],[0])
RTEMS_BSPOPTS_HELP([UARTS_IO_MODE],
[(BSP--console driver)
 Define to 0 or 1 if you want polled I/O performed by RTEMS.
 Define to 1 if you want interrupt-driven performed by RTEMS.
 Define to 2 if you want polled I/O performed by EPPCBug.
 There is no provision to have a mix of interrupt-driven and polled I/O
 ports, except that the printk port may use a different mode from the
 other ports. If this is done, do not open the printk port from an RTEMS
 application. With EPPCBug 1.1, if mode 2 is selected, CONSOLE_MINOR
 must be set to SMC1_MINOR. This is a deficiency of the firmware: it
 does not perform serial I/O on any port other than its default debug
 port, which must be SMC1.])

RTEMS_BSPOPTS_SET([PRINTK_MINOR],[*],[SMC2_MINOR])
RTEMS_BSPOPTS_HELP([PRINTK_MINOR],
[(BSP--console driver)
 Must be defined to be one of SMC1_MINOR, SMC2_MINOR, SCC2_MINOR,
 SCC3_MINOR, or SCC4_MINOR. Determines which device is used for output
 by printk(). If the port that printk() uses is also used for other
 I/O (e.g. if  PRINTK_MINOR == \$CONSOLE_MINOR), then both ports should
 use the same type of I/O, otherwise the drivers will likely conflict with
 each other.])

RTEMS_BSPOPTS_SET([DISPATCH_HANDLER_STAT],[*],[])
RTEMS_BSPOPTS_HELP([DISPATCH_HANDLER_STAT],
[used by irq/irq.c])

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)

# Explicitly list a Makefile here
AC_CONFIG_FILES([Makefile])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
