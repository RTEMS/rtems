## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-eth_comm],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

# FIXME: Explanation cloned from mbx8xx
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

RTEMS_BSPOPTS_SET([DISPATCH_HANDLER_STAT],[*],[])
RTEMS_BSPOPTS_HELP([DISPATCH_HANDLER_STAT],
[used by irq/irq.c])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
canbus/Makefile
console/Makefile
irq/Makefile
network/Makefile
start/Makefile
startup/Makefile
vectors/Makefile
wrapup/Makefile])

RTEMS_PPC_EXCEPTIONS([new])

AC_OUTPUT
