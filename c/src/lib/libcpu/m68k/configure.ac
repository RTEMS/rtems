## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c-src-lib-libcpu-m68k],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([m68040])
RTEMS_TOP(../../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.8.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_BARE
RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
AM_PROG_CC_C_O
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

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

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
shared/Makefile
m68040/Makefile
mcf5206/Makefile
])
AC_OUTPUT
