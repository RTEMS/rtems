## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-sparc-leon3],
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
[The leon3 console driver can operate in either polled or interrupt mode.
Under the simulator (especially when FAST_UART is defined), polled seems
to operate better.])

RTEMS_BSPOPTS_SET([BSP_LEON3_SMP],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_LEON3_SMP],
[Always defined when on a LEON3 to enable the LEON3 support for
 determining the CPU core number in an SMP configuration.])

RTEMS_BSPOPTS_SET([BSP_POWER_DOWN_AT_FATAL_HALT],[*],[])
RTEMS_BSPOPTS_HELP([BSP_POWER_DOWN_AT_FATAL_HALT],
[If defined, CPU is powered down on fatal exit. Otherwise generate system
 error which will hand over to debugger, simulator, etc.])

RTEMS_BSP_CLEANUP_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
