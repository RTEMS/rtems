# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libcpu-powerpc],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([mpc6xx])
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

AM_CONDITIONAL(shared, \
   test "$RTEMS_CPU_MODEL" = "" \
|| test "$RTEMS_CPU_MODEL" = "mpc750" \
|| test "$RTEMS_CPU_MODEL" = "mpc7400" \
|| test "$RTEMS_CPU_MODEL" = "mpc7455" \
|| test "$RTEMS_CPU_MODEL" = "mpc7457" \
|| test "$RTEMS_CPU_MODEL" = "mpc555" \
|| test "$RTEMS_CPU_MODEL" = "mpc55xx" \
|| test "$RTEMS_CPU_MODEL" = "mpc5xx" \
|| test "$RTEMS_CPU_MODEL" = "ppc603e" \
|| test "$RTEMS_CPU_MODEL" = "ppc403" \
|| test "$RTEMS_CPU_MODEL" = "ppc405" \
|| test "$RTEMS_CPU_MODEL" = "ppc440" \
|| test "$RTEMS_CPU_MODEL" = "mpc604" \
|| test "$RTEMS_CPU_MODEL" = "mpc6xx" \
|| test "$RTEMS_CPU_MODEL" = "mpc8xx" \
|| test "$RTEMS_CPU_MODEL" = "mpc821" \
|| test "$RTEMS_CPU_MODEL" = "mpc860" \
|| test "$RTEMS_CPU_MODEL" = "mpc8240" \
|| test "$RTEMS_CPU_MODEL" = "mpc8245" \
|| test "$RTEMS_CPU_MODEL" = "mpc8260" \
|| test "$RTEMS_CPU_MODEL" = "mpc83xx" \
|| test "$RTEMS_CPU_MODEL" = "qoriq" \
|| test "$RTEMS_CPU_MODEL" = "e500")

# test on CPU type
AM_CONDITIONAL(mpc505, test "$RTEMS_CPU_MODEL" = "mpc505")
AM_CONDITIONAL(mpc55xx, test "$RTEMS_CPU_MODEL" = "mpc55xx")
AM_CONDITIONAL(mpc5xx, test "$RTEMS_CPU_MODEL" = "mpc5xx" \
|| test "$RTEMS_CPU_MODEL" = "mpc555" )
AM_CONDITIONAL(mpc6xx, test "$RTEMS_CPU_MODEL" = "mpc6xx" \
|| test "$RTEMS_CPU_MODEL" = "mpc604" \
|| test "$RTEMS_CPU_MODEL" = "mpc7400" \
|| test "$RTEMS_CPU_MODEL" = "mpc7455" \
|| test "$RTEMS_CPU_MODEL" = "mpc7457" \
|| test "$RTEMS_CPU_MODEL" = "mpc750" \
|| test "$RTEMS_CPU_MODEL" = "mpc8240" \
|| test "$RTEMS_CPU_MODEL" = "mpc8245" \
|| test "$RTEMS_CPU_MODEL" = "mpc83xx" )
AM_CONDITIONAL(mpc8xx, test "$RTEMS_CPU_MODEL" = "mpc8xx" \
|| test "$RTEMS_CPU_MODEL" = "mpc821" \
|| test "$RTEMS_CPU_MODEL" = "mpc860" )
AM_CONDITIONAL(mpc8260, test "$RTEMS_CPU_MODEL" = "mpc8260")
AM_CONDITIONAL(mpc83xx, test "$RTEMS_CPU_MODEL" = "mpc83xx")
AM_CONDITIONAL(qoriq, test "$RTEMS_CPU_MODEL" = "qoriq")

# the ppc405 and ppc440 share files with the ppc403
AM_CONDITIONAL(ppc403, test "$RTEMS_CPU_MODEL" = "ppc403")
AM_CONDITIONAL(ppc405, test "$RTEMS_CPU_MODEL" = "ppc405")
AM_CONDITIONAL(ppc440, test "$RTEMS_CPU_MODEL" = "ppc440")
AM_CONDITIONAL(ppc4xx, test "$RTEMS_CPU_MODEL" = "ppc403" \
|| test "$RTEMS_CPU_MODEL" = "ppc405" \
|| test "$RTEMS_CPU_MODEL" = "ppc440")

AM_CONDITIONAL(e500, test "$RTEMS_CPU_MODEL" = "e500" \
|| test "$RTEMS_CPU_MODEL" = "qoriq" )

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
