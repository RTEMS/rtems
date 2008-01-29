## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-tests-sptests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([sp01])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.10])
AM_MAINTAINER_MODE


RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_RTEMS_TEST_NO_PAUSE

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)


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
sp30/Makefile
sp31/Makefile
sp32/Makefile
sp33/Makefile
sp34/Makefile
sp35/Makefile
sp36/Makefile
sp37/Makefile
sp38/Makefile
sp39/Makefile
sp40/Makefile
sp41/Makefile
sp42/Makefile
sp43/Makefile
spsize/Makefile
spfatal/Makefile
spfatal01/Makefile
spfatal02/Makefile
spfatal03/Makefile
spfatal04/Makefile
spfatal05/Makefile
spfatal06/Makefile
spfatal07/Makefile
spfatal08/Makefile
spfatal09/Makefile
spfatal10/Makefile
])
AC_OUTPUT
