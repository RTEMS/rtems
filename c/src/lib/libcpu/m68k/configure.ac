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
RTEMS_PROG_CCAS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

# At this time all models should use the shared directory so do this
AM_CONDITIONAL(shared, true)

AM_CONDITIONAL(m68040, test "$RTEMS_CPU_MODEL" = "m68040")

AM_CONDITIONAL(mcf5206, test "$RTEMS_CPU_MODEL" = "mcf5206")

AM_CONDITIONAL(mcpu32, test "$RTEMS_CPU_MODEL" = "m68340" \
|| test "$RTEMS_CPU_MODEL" = "m68331" \
|| test "$RTEMS_CPU_MODEL" = "m68332" \
|| test "$RTEMS_CPU_MODEL" = "m68336" \
|| test "$RTEMS_CPU_MODEL" = "m68337" \
|| test "$RTEMS_CPU_MODEL" = "mcpu32" \
)

AM_CONDITIONAL(mcpu32p, test "$RTEMS_CPU_MODEL" = "m68360" \
|| test "$RTEMS_CPU_MODEL" = "mcpu32p" \
)

AM_CONDITIONAL(mcf5272, test "$RTEMS_CPU_MODEL" = "mcf5272" )

case "$RTEMS_CPU_MODEL" in
mcf5206 ) AC_CONFIG_SUBDIRS([mcf5206]);;
*) ;;
esac

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
shared/Makefile
shared/cache/Makefile
shared/misc/Makefile
m68040/Makefile
m68040/fpsp/Makefile])
AC_OUTPUT
