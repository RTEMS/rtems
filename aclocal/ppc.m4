# $Id$
#
# Some hacks for handling powerpc-exception subdirectories
#
# Note: Consider this file a temporary band-aid until a better, more general
# subdirectory handling solution is introduced to RTEMS.

AC_DEFUN([RTEMS_PPC_EXCEPTIONS],
[
exceptions="$1_exception_processing"
AC_SUBST(exceptions)

AC_CONFIG_COMMANDS_POST(
[
 exceptions_subdirs="$1_exception_processing"
 updir=/../support

if test "$no_recursion" != yes; then

  RTEMS_CONFIGURE_ARGS_QUOTE([ac_sub_configure_args])

  for ac_subdir in : $exceptions_subdirs; do test "x$ac_subdir" = x: && continue

    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    test -d $srcdir${updir}/$ac_subdir || continue

    AC_MSG_NOTICE([configuring in $ac_subdir])
    case $srcdir in
    .) ;;
    *) AS_MKDIR_P(["./$ac_subdir"])
       if test -d ./$ac_subdir; then :;
       else
         AC_MSG_ERROR([cannot create `pwd`/$ac_subdir])
       fi
       ;;
    esac

    ac_popdir=`pwd`
    cd $ac_subdir

    # A "../" for each directory in /$ac_subdir.
    ac_dots=`echo $ac_subdir |
             sed 's,^\./,,;s,[[^/]]$,&/,;s,[[^/]]*/,../,g'`

    case $srcdir in
    .) # No --srcdir option.  We are building in place.
      ac_sub_srcdir=$srcdir${updir} ;;
    [[\\/]]* | ?:[[\\/]]* ) # Absolute path.
      ac_sub_srcdir=$srcdir${updir}/$ac_subdir ;;
    *) # Relative path.
      ac_sub_srcdir=$ac_dots$srcdir${updir}/$ac_subdir ;;
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
])
])
