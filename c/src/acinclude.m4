# $Id$
#
# Some hacks for handling special config subdirectories
#
# Note: Consider this file a temporary band-aid until a better, more general
# subdirectory handling solution is introduced to RTEMS.

AC_DEFUN([RTEMS_CONFIG_SUBDIR],
[
AC_CONFIG_COMMANDS_POST(
[
if test "$multilib" != "yes"; then
 ac_sub_sourcedir=$2
 ac_sub_builddir=$1

if test "$no_recursion" != yes; then

  # Remove --cache-file and --srcdir arguments so they do not pile up.
  ac_sub_configure_args=
  ac_prev=
  for ac_arg in $ac_configure_args; do
    if test -n "$ac_prev"; then
      ac_prev=
      continue
    fi
    case $ac_arg in
    -cache-file | --cache-file | --cache-fil | --cache-fi \
    | --cache-f | --cache- | --cache | --cach | --cac | --ca | --c)
      ac_prev=cache_file ;;
    -cache-file=* | --cache-file=* | --cache-fil=* | --cache-fi=* \
    | --cache-f=* | --cache-=* | --cache=* | --cach=* | --cac=* | --ca=* \
    | --c=*)
      ;;
    --config-cache | -C)
      ;;
    -srcdir | --srcdir | --srcdi | --srcd | --src | --sr)
      ac_prev=srcdir ;;
    -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
      ;;
    *) ac_sub_configure_args="$ac_sub_configure_args $ac_arg" ;;
    esac
  done

  ac_sub_configure_args="$ac_sub_configure_args $3"

  for ac_subdir in : $ac_sub_sourcedir; do test "x$ac_subdir" = x: && continue

    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    test -d $srcdir/$ac_subdir || continue

    AC_MSG_NOTICE([configuring in $ac_subdir])
    case $srcdir in
    .) ;;
    *) AS_MKDIR_P(["./$ac_sub_builddir"])
       if test -d ./$ac_sub_builddir; then :;
       else
         AC_MSG_ERROR([cannot create `pwd`/$ac_sub_builddir])
       fi
       ;;
    esac

    ac_popdir=`pwd`
    cd $ac_sub_builddir

    # A "../" for each directory in /$ac_subdir.
    ac_dots=`echo $ac_sub_builddir |
             sed 's,^\./,,;s,[[^/]]$,&/,;s,[[^/]]*/,../,g'`

    case $srcdir in
    .) # No --srcdir option.  We are building in place.
      ac_sub_srcdir=$srcdir${ac_subdir} ;;
    [[\\/]]* | ?:[[\\/]]* ) # Absolute path.
      ac_sub_srcdir=$srcdir/$ac_subdir ;;
    *) # Relative path.
      ac_sub_srcdir=$ac_dots$srcdir/$ac_subdir ;;
    esac

    # Check for guested configure; otherwise get Cygnus style configure.
    if test -f $ac_sub_srcdir/configure.gnu; then
      ac_sub_configure="$SHELL '$ac_sub_srcdir/configure.gnu'"
    elif test -f $ac_sub_srcdir/configure; then
      ac_sub_configure="$SHELL '$ac_sub_srcdir/configure'"
    elif test -f $ac_sub_srcdir/configure.in; then
      ac_sub_configure=$ac_configure
    else
      AC_MSG_WARN([no configuration information is in $ac_subdir])
      ac_sub_configure=
    fi

    # The recursion is here.
    if test -n "$ac_sub_configure"; then
      # Make the cache file name correct relative to the subdirectory.
      case $cache_file in
      [[\\/]]* | ?:[[\\/]]* ) ac_sub_cache_file=$cache_file ;;
      *) # Relative path.
        ac_sub_cache_file=$ac_dots$cache_file ;;
      esac

      AC_MSG_NOTICE([running $ac_sub_configure $ac_sub_configure_args --cache-file=$ac_sub_cache_file --srcdir=$ac_sub_srcdir])
      # The eval makes quoting arguments work.
      eval $ac_sub_configure $ac_sub_configure_args \
           --cache-file=$ac_sub_cache_file --srcdir=$ac_sub_srcdir ||
        AC_MSG_ERROR([$ac_sub_configure failed for $ac_subdir])
    fi

    cd $ac_popdir
  done
fi
fi
])
])
