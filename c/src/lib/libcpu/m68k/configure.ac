## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src-lib-libcpu-m68k],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([m68040])
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

AM_CONDITIONAL(shared, test "$RTEMS_CPU_MODEL" = "m68020" \
|| test "$RTEMS_CPU_MODEL" = "m68020" \
|| test "$RTEMS_CPU_MODEL" = "m68030" \
|| test "$RTEMS_CPU_MODEL" = "m68lc040" \
|| test "$RTEMS_CPU_MODEL" = "m68040" \
|| test "$RTEMS_CPU_MODEL" = "m68ec040" \
|| test "$RTEMS_CPU_MODEL" = "m68lc040" \
|| test "$RTEMS_CPU_MODEL" = "m68060" )

AM_CONDITIONAL(m68040, test "$RTEMS_CPU_MODEL" = "m68040")

AM_CONDITIONAL(mcf5206, test "$RTEMS_CPU_MODEL" = "mcf5206")

case "$RTEMS_CPU_MODEL" in
mcf5206 ) AC_CONFIG_SUBDIRS([mcf5206]);;
*) ;;
esac

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
shared/Makefile
shared/cache/Makefile
m68040/Makefile
m68040/fpsp/Makefile])
AC_OUTPUT
