dnl $Id$

## HACK: Work-around to structural issue with RTEMS
## The macros below violate most autoconf and canonicalization standards
AC_DEFUN(RTEMS_CONFIG_BUILD_SUBDIRS,
[
define([RTEMS_BUILD_SUBDIRS], 
ifdef([RTEMS_BUILD_SUBDIRS], [RTEMS_BUILD_SUBDIRS ],)[$1])dnl
build_subdirs="RTEMS_BUILD_SUBDIRS"
AC_SUBST(build_subdirs)
])

dnl Derived from automake-1.4's AC_OUTPUT_SUBDIRS
AC_DEFUN(RTEMS_OUTPUT_BUILD_SUBDIRS,
[
if test "$no_recursion" != yes; then
  if test $build_alias != $host_alias; then 
    target_subdir="$host_alias"
  else
    target_subdir="."
  fi
  # Remove --cache-file and --srcdir arguments so they do not pile up.
  ac_sub_configure_args=
  ac_prev=
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
    --target*) ;;
    --build*) ;;
    --host*) ;;
    *) ac_sub_configure_args="$ac_sub_configure_args $ac_arg" ;;
    esac
  done

  for ac_config_dir in $1; do

    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    if test ! -d $srcdir/$ac_config_dir; then
      continue
    fi

    _RTEMS_PUSH_BUILDDIR([$ac_config_dir])

    _RTEMS_SUB_SRCDIR([$ac_config_dir])

    # The recursion is here.
    if test -n "$ac_sub_configure"; then
      ac_sub_cache_file=./config.cache
      _RTEMS_GIVEN_INSTALL

      echo "[running ${CONFIG_SHELL-/bin/sh} $ac_sub_configure $ac_sub_configure_args --cache-file=$ac_sub_cache_file] --srcdir=$ac_sub_srcdir"
      # The eval makes quoting arguments work.
      if eval ${CONFIG_SHELL-/bin/sh} $ac_sub_configure \
	$ac_sub_configure_args \
	--srcdir=$ac_sub_srcdir \
	--with-target-subdir=$target_subdir \
        --cache-file=$ac_sub_cache_file 
      then :
      else
        AC_MSG_ERROR([$ac_sub_configure failed for $ac_config_dir])
      fi
    fi

    _RTEMS_POP_BUILDDIR
  done
fi
])
