## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-lm32-lm32_evr],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE




RTEMS_BSPOPTS_SET([LM32_ON_SIMULATOR],[lm32_evr_gdbsim],[1])
RTEMS_BSPOPTS_SET([LM32_ON_SIMULATOR],[*],[])
RTEMS_BSPOPTS_HELP([LM32_ON_SIMULATOR],
[If defined, this indicates the BSP is being built to run on the lm32
 simulator in GDB.  This enables fast idle support which speeds up the
 clock ticks while the idle task is running so time spent in the idle
 task is minimized.  This significantly reduces the wall time required
 to execute the RTEMS test suites.  It also enables a special exit and
 alternate printk support.])
AM_CONDITIONAL(LM32_ON_SIMULATOR,test x"$LM32_ON_SIMULATOR" = "x1")

RTEMS_BSP_CLEANUP_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
