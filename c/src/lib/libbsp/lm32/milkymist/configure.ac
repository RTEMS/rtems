## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-lm32-milkymist],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
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

RTEMS_BSPOPTS_SET([ON_SIMULATOR],[*],[])
RTEMS_BSPOPTS_HELP([ON_SIMULATOR],
[If defined, this indicates the BSP is being built to run on the lm32
 simulator in GDB.  This enables fast idle support which speeds up the
 clock ticks while the idle task is running so time spent in the idle
 task is minimized.  This significantly reduces the wall time required
 to execute the RTEMS test suites.  It also enables a special exit and
 alternate printk support.])

RTEMS_BSP_CLEANUP_OPTIONS(0,0)
# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
