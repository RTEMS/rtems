dnl $Id$

dnl
dnl Misc utility macros for subdir handling to work around missing abilities
dnl in autoconf, automake and structural issues with RTEMS
dnl
dnl Contains parts derived from autoconf-2.13 AC_OUTPUT_SUBDIRS and Cygnus'
dnl configure.in.
dnl

dnl
dnl _RTEMS_PUSH_BUILDDIR(SUBDIR)
dnl
AC_DEFUN(_RTEMS_PUSH_BUILDDIR,
[
# _RTEMS_PUSH_BUILDDIR
    echo configuring in $1
    case "$srcdir" in
    .) ;;
    *) AS_MKDIR_P([$1])
      ;;
    esac

    ac_popdir=`pwd`
    cd $1
])

dnl
dnl _RTEMS_POP_BUILDDIR
dnl
AC_DEFUN(_RTEMS_POP_BUILDDIR,
[
  cd $ac_popdir
])

dnl
dnl _RTEMS_GIVEN_INSTALL
dnl
AC_DEFUN(_RTEMS_GIVEN_INSTALL,
[
ifdef([AC_PROVIDE_AC_PROG_INSTALL],[  
  case "$ac_given_INSTALL" in
  [[\\/]]* | ?:[[\\/]]*) INSTALL="$ac_given_INSTALL" ;;
  *) INSTALL="$ac_dots$ac_given_INSTALL" ;;
  esac
])dnl
])

dnl
dnl _AC_DOTS(PATH)
dnl
AC_DEFUN(_AC_DOTS,[
# A "../" for each directory in $1.
    ac_dots=`echo $1 | \
             sed -e 's%^\./%%' -e 's%[[^/]]$%&/%' -e 's%[[^/]]*/%../%g'`
])

dnl
dnl _RTEMS_ADJUST_SRCDIR(REVAR,CONFIG_DIR[,TARGET_SUBDIR])
dnl
AC_DEFUN(_RTEMS_ADJUST_SRCDIR,[
    _AC_DOTS(ifelse([$3], ,[$2],[$3/$2]))

    case "$srcdir" in
    .) # No --srcdir option.  We are building in place.
      $1=$srcdir ;;
    [[\\/]]* | ?:[[\\/]]*) # Absolute path.
      $1=$srcdir/$2 ;;
    *) # Relative path.
      $1=$ac_dots$srcdir/$2 ;;
    esac
])

dnl
dnl _RTEMS_SUB_SRCDIR(AC_CONFIG_DIR[,TARGET_SUBDIR])
dnl
AC_DEFUN(_RTEMS_SUB_SRCDIR,[
# _RTEMS_SUB_SRCDIR
    _RTEMS_ADJUST_SRCDIR(ac_sub_srcdir,$1,$2)

    # Check for configure
    if test -f $ac_sub_srcdir/configure; then
      ac_sub_configure=$ac_sub_srcdir/configure
    else
      AC_MSG_WARN([no configuration information is in $1])
      ac_sub_configure=
    fi
])

AC_DEFUN(RTEMS_TARGET_CONFIG_PREPARE,[
# Record target_configdirs and the configure arguments in Makefile.
targargs=`echo "${ac_configure_args}" | \
        sed -e 's/--no[[^ 	]]*//' \
            -e 's/--cache[[a-z-]]*=[[^ 	]]*//' \
            -e 's/--ho[[a-z-]]*=[[^ 	]]*//' \
            -e 's/--bu[[a-z-]]*=[[^ 	]]*//' \
            -e 's/--ta[[a-z-]]*=[[^ 	]]*//'`;

targargs="--host=${target_alias} --build=${build} ${targargs}"
AC_SUBST(targargs)
])

AC_DEFUN(RTEMS_TARGET_CONFIG_SUBDIRS,[
#
# TARGET_CONFIG_SUBDIRS
#
if test "$no_recursion" != yes; then
 if test $target != $build; then
  # Remove --srcdir arguments so they do not pile up.
  ac_sub_configure_args=
  ac_prev=
  for ac_arg in $targargs; do
    if test -n "$ac_prev"; then
      ac_prev=
      continue
    fi
    case "$ac_arg" in
    -srcdir | --srcdir | --srcdi | --srcd | --src | --sr)
      ac_prev=srcdir ;;
    -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
      ;;
    *) ac_sub_configure_args="$ac_sub_configure_args $ac_arg" ;;
    esac
  done

  if test -n "$target_configdirs" && test -d $target_subdir; 
  then :;
  else
    AS_MKDIR_P(["$target_subdir"])
  fi

  for ac_config_dir in $target_configdirs; do
    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    if test ! -d $srcdir/$ac_config_dir; then
      continue
    fi

    _RTEMS_PUSH_BUILDDIR([$target_subdir/$ac_config_dir])

    _RTEMS_SUB_SRCDIR([$ac_config_dir],[$target_subdir])

    # The recursion is here.
    if test -n "$ac_sub_configure"; then

      # Make the cache file name correct relative to the subdirectory.
      ac_sub_cache_file=$cache_file
      _RTEMS_GIVEN_INSTALL

      echo "[running ${CONFIG_SHELL-/bin/sh} $ac_sub_configure $ac_sub_configure_args --cache-file=$ac_sub_cache_file] --srcdir=$ac_sub_srcdir"
      # The eval makes quoting arguments work.
      CC=${CC_FOR_TARGET}; export CC;
      if eval ${CONFIG_SHELL-/bin/sh} $ac_sub_configure \
	$ac_sub_configure_args --srcdir=$ac_sub_srcdir \
	--with-target-subdir=$target_subdir \
	--cache-file=$ac_sub_cache_file \
        --exec-prefix="\${prefix}/$target_subdir"
      then :
      else
        AC_MSG_ERROR([$ac_sub_configure failed for $ac_config_dir])
      fi
    fi

    _RTEMS_POP_BUILDDIR
  done
 fi
fi
])

AC_DEFUN(RTEMS_HOST_CONFIG_PREPARE,[
# Record host_configdirs and the configure arguments in Makefile.
hostargs=`echo "${ac_configure_args}" | \
        sed -e 's/--no[[^ 	]]*//' \
            -e 's/--cache[[a-z-]]*=[[^ 	]]*//' \
            -e 's/--ho[[a-z-]]*=[[^ 	]]*//' \
            -e 's/--bu[[a-z-]]*=[[^ 	]]*//' \
            -e 's/--ta[[a-z-]]*=[[^ 	]]*//'`;

hostargs="--host=${host_alias} --build=${build} --target=${target_alias} ${hostargs}"
AC_SUBST(hostargs)
])

AC_DEFUN(RTEMS_HOST_CONFIG_SUBDIRS,[
#
# HOST_CONFIG_SUBDIRS
#
if test "$no_recursion" != yes; then

 if test $build != $host; then
  # Remove --srcdir arguments so they do not pile up.
  ac_sub_configure_args=
  ac_prev=
  for ac_arg in $hostargs; do
    if test -n "$ac_prev"; then
      ac_prev=
      continue
    fi
    case "$ac_arg" in
    -srcdir | --srcdir | --srcdi | --srcd | --src | --sr)
      ac_prev=srcdir ;;
    -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
      ;;
    *) ac_sub_configure_args="$ac_sub_configure_args $ac_arg" ;;
    esac
  done

  if test -n "$host_configdirs" && test -d $host_subdir;
  then :;
  else
    AS_MKDIR_P(["$host_subdir"])
  fi

  for ac_config_dir in $host_configdirs; do
    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    if test ! -d $srcdir/$ac_config_dir; then
      continue
    fi

    _RTEMS_PUSH_BUILDDIR([$host_subdir/$ac_config_dir])

    _RTEMS_SUB_SRCDIR([$ac_config_dir],[$host_subdir])

    # The recursion is here.
    if test -n "$ac_sub_configure"; then

      # Make the cache file name correct relative to the subdirectory.
      ac_sub_cache_file=$cache_file
      _RTEMS_GIVEN_INSTALL

      echo "[running ${CONFIG_SHELL-/bin/sh} $ac_sub_configure $ac_sub_configure_args --cache-file=$ac_sub_cache_file] --srcdir=$ac_sub_srcdir"
      CC=${CC_FOR_HOST-$host_alias-gcc}; export CC;
      # The eval makes quoting arguments work.
      if eval ${CONFIG_SHELL-/bin/sh} $ac_sub_configure \
	$ac_sub_configure_args --srcdir=$ac_sub_srcdir \
	--with-target-subdir=$host_subdir \
	--cache-file=$ac_sub_cache_file
      then :
      else
        AC_MSG_ERROR([$ac_sub_configure failed for $ac_config_dir])
      fi
    fi

    _RTEMS_POP_BUILDDIR
  done
 fi
fi
])
