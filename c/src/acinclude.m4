# Some hacks for handling special config subdirectories
#
# Note: Consider this file a temporary band-aid until a better, more general
# subdirectory handling solution is introduced to RTEMS.

AC_DEFUN([_RTEMS_CONFIG_SUBDIR],
[
if test "$no_recursion" != yes; then
  ac_sub_sourcedir=$2
  ac_sub_builddir=$1

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

      AC_MSG_NOTICE([running $ac_sub_configure $ac_sub_configure_args $3 --cache-file=$ac_sub_cache_file --srcdir=$ac_sub_srcdir])
      # The eval makes quoting arguments work.
      eval $ac_sub_configure $ac_sub_configure_args $3 \
           --cache-file=$ac_sub_cache_file --srcdir=$ac_sub_srcdir ||
        AC_MSG_ERROR([$ac_sub_configure failed for $ac_subdir])
    fi

    cd "$ac_popdir"
  done
fi
])

## RTEMS_BSP_CONFIG_SUBDIR(builddir,srcdir,configargs,condition)
AC_DEFUN([RTEMS_BSP_CONFIG_SUBDIR],[
m4_expand_once([RTEMS_CONFIGURE_ARGS_QUOTE([ac_sub_configure_args])])
AS_IF([$4],[BSP_SUBDIRS="$BSP_SUBDIRS $1"])
AC_CONFIG_COMMANDS_POST([
AS_IF([$4],[_RTEMS_CONFIG_SUBDIR([$1],[$2],[$3])])])
])
