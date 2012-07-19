## Process this file with autoconf to produce a configure script.
##

AC_PREREQ([2.69])
AC_INIT([rtems-c],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([TOOL_TARGETS])
RTEMS_TOP(..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

## These options are used within this file.
RTEMS_ENABLE_RTEMSBSP

# Set up rtems_bsp
AS_IF([test x"$enable_rtemsbsp" = x"no"],[
# --disable-rtemsbsp
  rtems_bsp=""
],[
  AS_IF([test -z "$enable_rtemsbsp"],
    [# --enable-rtemsbsp=""
    RTEMS_CHECK_BSPS(rtems_bsp)],
    [# --enable-rtemsbsp="list"
    rtems_bsp="$enable_rtemsbsp"])
])

# Check sanity of BSPs in $rtems_bsp
for _rtems_bsp in : $rtems_bsp; do test "x$_rtems_bsp" = x: && continue
  # make sure there is a make/custom file for the bsp
  _RTEMS_CHECK_CUSTOM_BSP([$_rtems_bsp.cfg],[bsp_cfg])

  AS_IF([test -n "$bsp_cfg"],
  [
    # retrieve bsp_family
    RTEMS_BSP_ALIAS([$_rtems_bsp],[rtems_bsp_family])

    # Is there a configure script for the BSP?
    AS_IF([test -r "$srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU/$rtems_bsp_family/configure"],
      [RTEMS_BSP_LIST="$RTEMS_BSP_LIST $_rtems_bsp"])
  ])
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
rtems_bsp_configure="$rtems_bsp_configure '--with-project-root=../../'"
rtems_bsp_configure="$rtems_bsp_configure '--with-project-top=../../'"
AC_SUBST(rtems_bsp_configure)

AC_SUBST(RTEMS_BSP_LIST)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile],[
for bsp in : $RTEMS_BSP_LIST; do test "x$bsp" = x: && continue
cat >> Makefile << BSPEOF

$bsp: src/configure
	@set fnord \$(MAKEFLAGS); amf=\[$]\[$]2; \\
	  echo "Configuring RTEMS_BSP=$bsp"; \\
	  \$(MKDIR_P) $bsp; \\
	  ( cd $bsp && \\
	    CONFIG_SHELL=\$(SHELL) RTEMS_BSP=$bsp \$(rtems_bsp_configure) ) \\
	  || case "\$\$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac;
BSPEOF
done
],[RTEMS_BSP_LIST="$RTEMS_BSP_LIST"])
AC_OUTPUT
AS_IF([test -n "$RTEMS_BSP_LIST"],[
echo
echo target architecture: $target_cpu.
echo available BSPs: $RTEMS_BSP_LIST.
echo \'${MAKE} all\' will build the following BSPs: $RTEMS_BSP_LIST.
echo other BSPs can be built with \'${MAKE} RTEMS_BSP=\"bsp1 bsp2 ...\"\'
echo
])
