## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-powerpc-mpc8260,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)
RTEMS_CANONICAL_HOST

RTEMS_BSPOPTS_SET([DATA_CACHE_ENABLE],[*],[0])
RTEMS_BSPOPTS_HELP([DATA_CACHE_ENABLE],
[If defined, the data cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([INSTRUCTION_CACHE_ENABLE],[*],[0])
RTEMS_BSPOPTS_HELP([INSTRUCTION_CACHE_ENABLE],
[If defined, the instruction cache will be enabled after address translation
 is turned on.])

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

AM_CONFIG_HEADER(include/bspopts.h)

RTEMS_PROJECT_ROOT
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

# Explicitly list a Makefile here 
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
network/Makefile
irq/Makefile
vectors/Makefile
start/Makefile
startup/Makefile
wrapup/Makefile])

RTEMS_PPC_EXCEPTIONS([new])

AC_OUTPUT
