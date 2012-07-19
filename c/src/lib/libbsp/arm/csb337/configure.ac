## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-csb337],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
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

RTEMS_BSPOPTS_SET([csb637],[kit637_v6],[1])
RTEMS_BSPOPTS_SET([csb637],[csb637],[1])
RTEMS_BSPOPTS_SET([csb637],[*],[0])
RTEMS_BSPOPTS_HELP([csb637],
[If defined, this indicates that the BSP is being built for the
 csb637 variant.])

RTEMS_BSPOPTS_SET([ENABLE_LCD],[kit637_v6],[0])
RTEMS_BSPOPTS_SET([ENABLE_LCD],[*],[0])
RTEMS_BSPOPTS_HELP([ENABLE_LCD],
[If defined, enable use of the SED1356 controller and LCD.])
AM_CONDITIONAL(ENABLE_LCD,test "$ENABLE_LCD" = "1")

RTEMS_BSPOPTS_SET([ENABLE_UMON],[*],[1])
RTEMS_BSPOPTS_HELP([ENABLE_UMON],
[If defined, enable use of the uMon console.])
AM_CONDITIONAL(ENABLE_UMON,test "$ENABLE_UMON" = "1")

RTEMS_BSPOPTS_SET([ENABLE_UMON_CONSOLE],[*],[1])
RTEMS_BSPOPTS_HELP([ENABLE_UMON_CONSOLE],
[If defined, enable use of the MicroMonitor console device.])
AM_CONDITIONAL(ENABLE_UMON_CONSOLE,test "$ENABLE_UMON_CONSOLE" = "1")

## If ENABLE_UMON is 0 and ENABLE_UMON_CONSOLE is 1, then it is an error.
## TODO: Catch this error.

RTEMS_BSPOPTS_SET([ENABLE_USART0],[*],[1])
RTEMS_BSPOPTS_HELP([ENABLE_USART0],
[If defined, enable use of the USART 0.])
AM_CONDITIONAL(ENABLE_USART0,test "$ENABLE_USART0" = "1")

RTEMS_BSPOPTS_SET([ENABLE_USART1],[*],[1])
RTEMS_BSPOPTS_HELP([ENABLE_USART1],
[If defined, enable use of the USART 1.])
AM_CONDITIONAL(ENABLE_USART1,test "$ENABLE_USART1" = "1")

RTEMS_BSPOPTS_SET([ENABLE_USART2],[*],[1])
RTEMS_BSPOPTS_HELP([ENABLE_USART2],
[If defined, enable use of the USART 2.])
AM_CONDITIONAL(ENABLE_USART2,test "$ENABLE_USART2" = "1")

RTEMS_BSPOPTS_SET([ENABLE_USART3],[*],[1])
RTEMS_BSPOPTS_HELP([ENABLE_USART3],
[If defined, enable use of the USART 3.])
AM_CONDITIONAL(ENABLE_USART3,test "$ENABLE_USART3" = "1")

RTEMS_BSP_CLEANUP_OPTIONS(1, 1)
RTEMS_BSP_LINKCMDS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
