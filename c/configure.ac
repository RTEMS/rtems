## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-c],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([src])
RTEMS_TOP(..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.8])
AM_MAINTAINER_MODE

## These options are used within this file.
RTEMS_ENABLE_BARE
RTEMS_ENABLE_RTEMSBSP(rtems_bsp)

RTEMS_CHECK_CPU

AS_IF([test -z "$rtems_bsp"],
  [RTEMS_CHECK_BSPS(rtems_bsp)])

for i in $rtems_bsp; do

    # make sure there is a make/custom file for the bsp
    RTEMS_CHECK_CUSTOM_BSP(i)
 
    RTEMS_BSP_ALIAS($i,bspdir)

    # Is there code where there should be for this BSP?
    # The bare bsp is a special case as it is not under the RTEMS_CPU path
    case $bspdir in
      bare)
        bspcpudir=
        ;;
      *)
        bspcpudir=$RTEMS_CPU/
        ;;
    esac
     
    if test -d "$srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$bspcpudir$bspdir"; then
      RTEMS_BSP_LIST="$RTEMS_BSP_LIST $i"
    else
      AC_MSG_ERROR([unable to find libbsp directory ($bspdir) for  $i])
    fi
done

#
# Compose the configuration arguments to be passed to c/src/configure
#
 ##
## Partially borrowed from autoconf-2.13
##

## Adjust paths
_RTEMS_ADJUST_SRCDIR([rtems_bsp_configure],[src])
rtems_bsp_configure="$rtems_bsp_configure/configure"
  RTEMS_CONFIGURE_ARGS_QUOTE([rtems_bsp_configure_args],
    [-enable-rtemsbsp=* | --enable-rtemsbsp=* ) ;;])

rtems_bsp_configure="$rtems_bsp_configure $rtems_bsp_configure_args"
AC_SUBST(rtems_bsp_configure)

AC_SUBST(RTEMS_BSP_LIST)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo
echo target architecture: $target_cpu.
echo available BSPs: $rtems_bsp.
echo \'${MAKE} all\' will build the following BSPs: $RTEMS_BSP_LIST.
echo other BSPs can be built with \'${MAKE} RTEMS_BSP=\"bsp1 bsp2 ...\"\'
echo
