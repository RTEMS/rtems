## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)
AC_CONFIG_AUX_DIR(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE(rtems-c-src-lib-libbsp-sh-shsim,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS

RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)
RTEMS_CANONICAL_HOST

RTEMS_BSP_ARG_ENABLE([console],
[  --enable-console=[[null|gdbsci1]]],[
  case $enable_console in
  null)
    BSP_CONSOLE_DEVNAME=\"/dev/null\"
    BSP_CONSOLE_DRIVER_TABLE_ENTRY=DEVNULL_DRIVER_TABLE_ENTRY
    ;;
# Not yet
#  trap34)
#    BSP_CONSOLE_DEVNAME=\"/dev/trap34\"
#    BSP_CONSOLE_DRIVER_TABLE_ENTRY=DEVTRAP34_DRIVER_TABLE_ENTRY
#    ;;
  yes|gdbsci1)
    BSP_CONSOLE_DEVNAME=\"/dev/gdbsci1\"
    BSP_CONSOLE_DRIVER_TABLE_ENTRY=DEVGDBSCI_DRIVER_TABLE_ENTRY
    ;;
  *) AC_MSG_WARN([bad value $enable_console for --enable-console=<value>])
    ;;
  esac],[
    BSP_CONSOLE_DEVNAME=\"/dev/gdbsci1\"
    BSP_CONSOLE_DRIVER_TABLE_ENTRY=DEVGDBSCI_DRIVER_TABLE_ENTRY
])
AC_DEFINE_UNQUOTED(
  [BSP_CONSOLE_DEVNAME],
  [${BSP_CONSOLE_DEVNAME}],
  [device name to redirect the console to])
AC_DEFINE_UNQUOTED(
  [BSP_CONSOLE_DRIVER_TABLE_ENTRY],
  [${BSP_CONSOLE_DRIVER_TABLE_ENTRY}],
  [Driver table entry for the console device])

##
RTEMS_BSP_ARG_ENABLE([iomem-patch],
[  --enable-iomem-patch=[[yes|no]]])
if test x"$enable_iomem_patch" = x"yes"; then
AC_DEFINE_UNQUOTED(
  [HAVE_SHSIM_IOMEM_PATCH],
  [1],
  [whether support for functional IOMEM in shsim/gdb shall be enabled])
fi

## bsp-specific options
CPU_CLOCK_RATE_HZ=${CPU_CLOCK_RATE_HZ-20000000}
AC_SUBST(CPU_CLOCK_RATE_HZ)
AC_DEFINE_UNQUOTED(
  [CPU_CLOCK_RATE_HZ],
  [$CPU_CLOCK_RATE_HZ],
  [cpu clock rate in HZ])

## Used in scitab/Makefile.am
AC_SUBST(CPU_CLOCK_RATE_HZ)

AM_CONFIG_HEADER(include/bspopts.h)
RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
clock/Makefile
console/Makefile
include/Makefile
scitab/Makefile
gdbsci/Makefile
start/Makefile
startup/Makefile
wrapup/Makefile])
AC_OUTPUT
