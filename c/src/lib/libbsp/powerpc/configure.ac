## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([dmv177])
RTEMS_TOP(../../../../..)
AC_CONFIG_AUX_DIR(../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-powerpc,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)

RTEMS_PROJECT_ROOT

RTEMS_CHECK_BSPDIR([$RTEMS_BSP])

AM_CONDITIONAL(need_shared, test "$RTEMS_BSP_FAMILY" = "motorola_powerpc")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
shared/Makefile
shared/bootloader/Makefile
shared/clock/Makefile
shared/console/Makefile
shared/include/Makefile
shared/irq/Makefile
shared/motorola/Makefile
shared/openpic/Makefile
shared/pci/Makefile
shared/residual/Makefile
shared/start/Makefile
shared/startup/Makefile
shared/vectors/Makefile])
AC_OUTPUT
