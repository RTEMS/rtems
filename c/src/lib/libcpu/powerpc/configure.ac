## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libcpu-powerpc],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([mpc6xx])
RTEMS_TOP(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.8])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP


RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

AM_CONDITIONAL(shared, test "$RTEMS_CPU_MODEL" = "mpc750" \
|| test "$RTEMS_CPU_MODEL" = "mpc7400" \
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
|| test "$RTEMS_CPU_MODEL" = "mpc7400" \
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
mpc8260/Makefile
shared/Makefile
wrapup/Makefile])
AC_OUTPUT
