## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([src])
RTEMS_TOP(..)
AC_CONFIG_AUX_DIR(..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

## These options are used within this file.
RTEMS_ENABLE_BARE
RTEMS_ENABLE_RTEMSBSP(rtems_bsp)

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

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

## Remove --cache-file, --srcdir and --enable-rtemsbsp arguments
## so they do not pile up
  rtems_bsp_configure_args=
  for ac_arg in $ac_configure_args; do
    if test -n "$ac_prev"; then
      ac_prev=
      continue
    fi
    case "$ac_arg" in
    -cache-file | --cache-file | --cache-fil | --cache-fi \
    | --cache-f | --cache- | --cache | --cach | --cac | --ca | --c)
      ac_prev=cache_file ;;
    -cache-file=* | --cache-file=* | --cache-fil=* | --cache-fi=* \
    | --cache-f=* | --cache-=* | --cache=* | --cach=* | --cac=* | --ca=* | --c=*)
      ;;
    -srcdir | --srcdir | --srcdi | --srcd | --src | --sr)
      ac_prev=srcdir ;;
    -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
      ;;
    -enable-rtemsbsp=* | --enable-rtemsbsp=*) ;;
    -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
      ;;
    *) rtems_bsp_configure_args="$rtems_bsp_configure_args $ac_arg" ;;
    esac
  done

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
