## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-lib-libcpu-powerpc],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([mpc6xx])
RTEMS_TOP(../../../../..)
AC_CONFIG_AUX_DIR(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)

AM_CONDITIONAL(shared, test "$RTEMS_CPU_MODEL" = "mpc750" \
|| test "$RTEMS_CPU_MODEL" = "ppc603e" \
|| test "$RTEMS_CPU_MODEL" = "mpc604" \
|| test "$RTEMS_CPU_MODEL" = "mpc6xx" \
|| test "$RTEMS_CPU_MODEL" = "mpc821" \
|| test "$RTEMS_CPU_MODEL" = "mpc860" \
|| test "$RTEMS_CPU_MODEL" = "mpc8260" \
|| test "$RTEMS_CPU_MODEL" = "mpc8xx" )

## test on CPU type
AM_CONDITIONAL(mpc505, test "$RTEMS_CPU_MODEL" = "mpc505")
AM_CONDITIONAL(mpc6xx, test "$RTEMS_CPU_MODEL" = "mpc6xx" \
|| test "$RTEMS_CPU_MODEL" = "mpc604" \
|| test "$RTEMS_CPU_MODEL" = "mpc750" )
AM_CONDITIONAL(mpc8xx, test "$RTEMS_CPU_MODEL" = "mpc8xx" \
|| test "$RTEMS_CPU_MODEL" = "mpc821" \
|| test "$RTEMS_CPU_MODEL" = "mpc860" )
AM_CONDITIONAL(mpc8260, test "$RTEMS_CPU_MODEL" = "mpc8260")
AM_CONDITIONAL(ppc403, test "$RTEMS_CPU_MODEL" = "ppc403")
AM_CONDITIONAL(ppc405, test "$RTEMS_CPU_MODEL" = "ppc405")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
mpc505/Makefile
mpc505/timer/Makefile
mpc505/vectors/Makefile
mpc505/ictrl/Makefile
mpc8xx/Makefile
mpc8xx/clock/Makefile
mpc8xx/console-generic/Makefile
mpc8xx/cpm/Makefile
mpc8xx/exceptions/Makefile
mpc8xx/include/Makefile
mpc8xx/mmu/Makefile
mpc8xx/timer/Makefile
ppc403/Makefile
ppc403/clock/Makefile
ppc403/console/Makefile
ppc403/tty_drv/Makefile
ppc403/ictrl/Makefile
ppc403/timer/Makefile
ppc403/vectors/Makefile
mpc6xx/Makefile
mpc6xx/clock/Makefile
mpc6xx/timer/Makefile
mpc6xx/exceptions/Makefile
mpc6xx/mmu/Makefile
mpc6xx/timer/Makefile
mpc6xx/wrapup/Makefile
mpc8260/Makefile
mpc8260/clock/Makefile
mpc8260/console-generic/Makefile
mpc8260/include/Makefile
mpc8260/timer/Makefile
mpc8260/exceptions/Makefile
mpc8260/mmu/Makefile
mpc8260/cpm/Makefile
shared/Makefile
shared/include/Makefile
shared/src/Makefile
wrapup/Makefile])
AC_OUTPUT
