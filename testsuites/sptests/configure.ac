## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([sp01])
RTEMS_TOP(../../../..)
AC_CONFIG_AUX_DIR(../../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-tests-sptests,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_ENABLE_GCC28
RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_RTEMS_TEST_NO_PAUSE
RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

AC_SUBST(RTEMS_USE_GCC272)
AC_SUBST(BARE_CPU_CFLAGS)
AC_SUBST(BARE_CPU_MODEL)
AC_SUBST(CC_CFLAGS_DEFAULT)
AC_SUBST(CC_CFLAGS_DEBUG_V)
AC_SUBST(CC_CFLAGS_PROFILE_V)
AC_SUBST(CC_LDFLAGS_PROFILE_V)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
sp01/Makefile
sp02/Makefile
sp03/Makefile
sp04/Makefile
sp05/Makefile
sp06/Makefile
sp07/Makefile
sp08/Makefile
sp09/Makefile
sp11/Makefile
sp12/Makefile
sp13/Makefile
sp14/Makefile
sp15/Makefile
sp16/Makefile
sp17/Makefile
sp19/Makefile
sp20/Makefile
sp21/Makefile
sp22/Makefile
sp23/Makefile
sp24/Makefile
sp25/Makefile
sp26/Makefile
sp27/Makefile
sp28/Makefile
sp29/Makefile
spsize/Makefile
])
AC_OUTPUT

# spfatal/Makefile
