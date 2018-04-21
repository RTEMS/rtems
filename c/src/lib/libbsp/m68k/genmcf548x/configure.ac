## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-m68k-genmcf548x],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE




RTEMS_BSPOPTS_SET([BSP_CPU_CLOCK_SPEED],[COBRA5475],[132000000])
RTEMS_BSPOPTS_SET([BSP_CPU_CLOCK_SPEED],[m5484FireEngine],[100000000])
RTEMS_BSPOPTS_SET([BSP_CPU_CLOCK_SPEED],[*],[100000000])
RTEMS_BSPOPTS_HELP([BSP_CPU_CLOCK_SPEED],
[The bus clock to be used inside the mcf54xx])

RTEMS_BSPOPTS_SET([M5484FIREENGINE],[m5484FireEngine],[1])
RTEMS_BSPOPTS_SET([M5484FIREENGINE],[*],[])
RTEMS_BSPOPTS_HELP([M5484FIREENGINE],
[If defined, use custom settings for the m5484FireEngine BSP.])

RTEMS_BSPOPTS_SET([BSP_CONSOLE_BAUD],[COBRA5475],[19200])
RTEMS_BSPOPTS_SET([BSP_CONSOLE_BAUD],[*],[9600])
RTEMS_BSPOPTS_HELP([BSP_CONSOLE_BAUD],[initial baudrate for UARTs])

RTEMS_BSPOPTS_SET([HAS_DBUG],[COBRA5475],[1])
RTEMS_BSPOPTS_SET([HAS_DBUG],[*],[])
RTEMS_BSPOPTS_HELP([HAS_DBUG],
[If defined, we will not boot from RESET, but from Freescale DBug monitor.])

RTEMS_BSPOPTS_SET([HAS_LOW_LEVEL_INIT],[m5484FireEngine],[1])
RTEMS_BSPOPTS_SET([HAS_LOW_LEVEL_INIT],[*],[])
RTEMS_BSPOPTS_HELP([HAS_LOW_LEVEL_INIT],
[If defined, we will do all the low level init of the chip (like bus/memory...).])

RTEMS_BSP_CLEANUP_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
