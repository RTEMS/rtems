## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libcpu-m68k],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([m68040])
RTEMS_TOP([../../../../..],[../../..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
AM_PROG_CC_C_O
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_TOOL([OBJCOPY],[objcopy])

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
AM_CONDITIONAL(mcf5282, test "$RTEMS_CPU_MODEL" = "mcf5282" )
AM_CONDITIONAL(mcf5235, test "$RTEMS_CPU_MODEL" = "mcf5235" )
AM_CONDITIONAL(mcf5223x, test "$RTEMS_CPU_MODEL" = "mcf5223x" )
AM_CONDITIONAL(mcf5225x, test "$RTEMS_CPU_MODEL" = "mcf52258" )
AM_CONDITIONAL(mcf532x, test "$RTEMS_CPU_MODEL" = "mcf532x" )
AM_CONDITIONAL(mcf548x, test "$RTEMS_CPU_MODEL" = "mcf548x" )

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")


# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
