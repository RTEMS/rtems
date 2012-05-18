## HACK: Work-around to structural issue with RTEMS
## The macros below violate most autoconf and canonicalization standards
AC_DEFUN([RTEMS_CONFIG_BUILD_SUBDIRS],
[AC_REQUIRE([_RTEMS_OUTPUT_BUILD_SUBDIRS])
RTEMS_BUILD_SUBDIRS="$RTEMS_BUILD_SUBDIRS [$1]"
])

dnl Derived from automake-1.4's AC_OUTPUT_SUBDIRS
AC_DEFUN([_RTEMS_OUTPUT_BUILD_SUBDIRS],
[AC_CONFIG_COMMANDS([bsp-tools],
[
# HACK: This is a blantant hack and breaks Canadian crosses
build_alias="$build_alias"
host_alias="$build_alias"
if test "$no_recursion" != yes; then
  if test x"$build_alias" != x"$host_alias"; then 
    target_subdir="$host_alias"
  else
    target_subdir="."
  fi
  RTEMS_CONFIGURE_ARGS_QUOTE([ac_sub_configure_args],
    [
    -host* | --host* ) ;;
    --host ) ac_prev=host_alias;;
    -target* | --target* ) ;;
    -target ) ac_prev=target_alias ;;
    -build* | --build* ) ;;
    -build ) ac_prev_build_alias ;;
    CFLAGS=* );; # HACK: Strip CFLAGS
    *_alias=* ) ;; # HACK: Workaround to autoconf passing *_alias
    ],
    [rtems_configure_args])

  for rtems_config_dir in : $RTEMS_BUILD_SUBDIRS; do test "x$rtems_config_dir" = x: && continue

    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    if test ! -d $srcdir/$rtems_config_dir; then
      continue
    fi

    _RTEMS_PUSH_BUILDDIR([$rtems_config_dir])

    _RTEMS_SUB_SRCDIR([$rtems_config_dir])

    # The recursion is here.
    if test -n "$ac_sub_configure"; then
#      ac_sub_cache_file=./config.cache
      ac_sub_cache_file=/dev/null
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
        AC_MSG_ERROR([$ac_sub_configure failed for $rtems_config_dir])
      fi
    fi

    _RTEMS_POP_BUILDDIR
  done
fi],
[
RTEMS_BUILD_SUBDIRS="$RTEMS_BUILD_SUBDIRS"
rtems_configure_args="$ac_configure_args"
])
])

dnl
dnl Misc utility macros for subdir handling to work around missing abilities
dnl in autoconf, automake and structural issues with RTEMS
dnl
dnl Contains parts derived from autoconf-2.13 AC_OUTPUT_SUBDIRS and Cygnus'
dnl configure.ac.
dnl

dnl
dnl _RTEMS_PUSH_BUILDDIR(SUBDIR)
dnl
AC_DEFUN([_RTEMS_PUSH_BUILDDIR],
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
AC_DEFUN([_RTEMS_POP_BUILDDIR],
[
  cd "$ac_popdir"
])

dnl
dnl _RTEMS_GIVEN_INSTALL
dnl
AC_DEFUN([_RTEMS_GIVEN_INSTALL],
[
ifdef([AC_PROVIDE_AC_PROG_INSTALL],[  
  case "$ac_given_INSTALL" in
  [[\\/]]* | ?:[[\\/]]*) INSTALL="$ac_given_INSTALL" ;;
  *) INSTALL="$ac_dots$ac_given_INSTALL" ;;
  esac
])dnl
])

dnl
dnl _RTEMS_SUB_SRCDIR(AC_CONFIG_DIR[,TARGET_SUBDIR])
dnl
AC_DEFUN([_RTEMS_SUB_SRCDIR],[
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
