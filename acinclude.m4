## All directories belong in one of 3 categories.
## ${HOST_CONFIGDIRS_LIST} is directories we build using the host tools.
## ${TARGET_CONFIGDIRS_LIST} is directories we build using the target tools.
## ${BUILD_CONFIGDIRS_LIST} is directories we build using the build tools

AC_PREREQ(2.60)

dnl RTEMS_ARG_VAR(VAR,HELP-STRING)
dnl An internal macros to have help strings pretty
dnl printed with configure --help, without letting autoconf spoil config subdir
dnl environments by trying to propagate them
dnl Stripped down version of autoconf-2.52's AC_ARG_VAR.

AC_DEFUN([_RTEMS_ARG_VAR],
[
m4_expand_once([m4_divert_once([HELP_VAR],
[AS_HELP_STRING([$1], [$2], [              ])])],
      [$0($1)])dnl
])

AC_DEFUN([_RTEMS_SUBST_IFNOT],
[AS_IF([`echo " [$]$1 " | grep " $2 " > /dev/null`],
  [],
  [AC_SUBST([$1],["[$]$1 $2"])])
])

AC_DEFUN([_RTEMS_TOOLS],
[
m4_ifdef([_RTEMS_$2_CONFIGDIRS_LIST],
[
_RTEMS_ARG_VAR([CC_FOR_$2],
  [c-compiler to be used for $1 subdirs (default: auto-detected)])
_RTEMS_ARG_VAR([CFLAGS_FOR_$2],
  [c-flags to be used for $1 subdirs (default: provided by autoconf)])
_RTEMS_ARG_VAR([CXX_FOR_$2],
  [c++-compiler to be used for $1 subdirs (default: auto-detected)])
_RTEMS_ARG_VAR([CXXFLAGS_FOR_$2],
  [c++-flags to be used for $1 subdirs (default: provided by autoconf)])
])
])

AC_DEFUN([_RTEMS_COMMANDS_POST_CONFIG_SUBDIRS],
[
AC_CONFIG_COMMANDS_PRE([

if test -z "${build_alias}"; then
# build_alias is empty
  if test -z "${host_alias}"; then
  # host_alias is empty
    if test -z "${target_alias}"; then
    : target_alias is empty
    else
    : target_alias is not empty
    fi
  else
  # host_alias is not empty
    if test -z "${target_alias}"; then
    : target_alias is empty
    target_alias="${host_alias}"
    else
    : target_alias is not empty
    fi
  fi
else
# build_alias is not empty
  if test -z "${host_alias}"; then
  # host_alias is empty
    if test -z "${target_alias}"; then
    : target_alias is empty
    else
    : target_alias is not empty
    fi
  else
  # host_alias is not empty
    if test -z "${target_alias}"; then
    : target_alias is empty
    target_alias="${host_alias}"
    else
    : target_alias is not empty
    fi
  fi
fi

_RTEMS_BUILD_CONFIG_PREPARE
_RTEMS_HOST_CONFIG_PREPARE
_RTEMS_TARGET_CONFIG_PREPARE

build_subdir="."
build_SUBDIRS="${build_configdirs}"
build_configdirs="${build_configdirs}"

AS_IF([test $build != $host],
[dnl
  host_subdir="${host_alias-$host}"
  host_SUBDIRS=`echo "${host_configdirs}" | \
        sed -e "s%\([[^ ]][[^ ]]*\)%$host_subdir/\1%g"`
  host_configdirs="${host_configdirs}"
],[dnl
  host_SUBDIRS="${host_configdirs}"
  host_configdirs="${host_configdirs}"
])

AS_IF([test $build != $target],
[dnl
  target_subdir="${target_alias-$target}"
  target_SUBDIRS=`echo "${target_configdirs}" | \
      sed -e "s%\([[^ ]][[^ ]]*\)%$target_subdir/\1%g"`
  target_configdirs="${target_configdirs}"
],[dnl
  target_SUBDIRS="${target_configdirs}"
  target_configdirs="${target_configdirs}"
])

AC_SUBST(host_SUBDIRS)
AC_SUBST(target_SUBDIRS)
AC_SUBST(build_SUBDIRS)
])

  AC_CONFIG_COMMANDS_POST([
    _RTEMS_OUTPUT_SUBDIRS([build],[BUILD])
    _RTEMS_OUTPUT_SUBDIRS([host],[HOST])
    _RTEMS_OUTPUT_SUBDIRS([target],[TARGET])
  ])
])

dnl _RTEMS_SRCPATHS(BUILD-DIR-NAME)
dnl ----------------------------
dnl Inputs:
dnl   - BUILD-DIR-NAME is `top-build -> build' and `top-src -> src'
dnl   - `$srcdir' is `top-build -> top-src'
dnl
dnl Outputs:
dnl - `ac_builddir' is `.', for symmetry only.
dnl - `ac_top_builddir' is `build -> top_build'.
dnl      If not empty, has a trailing slash.
dnl - `ac_srcdir' is `build -> src'.
dnl - `ac_top_srcdir' is `build -> top-src'.
dnl
dnl and `ac_buildpath' etc., the absolute paths.
m4_define([_RTEMS_SRCPATHS],
[
m4_if([$2],,
[dstdir=$1],
[case "$2" in
"." | "" ) 	# No subdir was given
  dstdir=$1;;
* ) 		# A subdir was given
  dstdir=$2/$1;;
esac])
ac_builddir=.

AS_IF([test $dstdir != .],
[ # Strip off leading ./
  ac_builddir_suffix=/`echo $dstdir | sed 's,^\.[[\\/]],,'`
  ac_srcdir_suffix=/`echo $1 | sed 's,^\.[[\\/]],,'`
  # A "../" for each directory in $ac_dir_suffix.
  ac_top_builddir=`echo "$ac_builddir_suffix" | sed 's,/[[^\\/]]*,../,g'`
],[
  ac_dir_suffix= ac_top_builddir=
])

case $srcdir in
  .)  # No --srcdir option.  We are building in place.
    ac_srcdir=.
    if test -z "$ac_top_builddir"; then
       ac_top_srcdir=.
    else
       ac_top_srcdir=`echo $ac_top_builddir | sed 's,/$,,'`
    fi ;;
  [[\\/]]* | ?:[[\\/]]* )  # Absolute path.
    ac_srcdir=$srcdir$ac_srcdir_suffix;
    ac_top_srcdir=$srcdir;
    ;;
  *) # Relative path.
    ac_srcdir=$ac_top_builddir$srcdir$ac_srcdir_suffix;
    ac_top_srcdir=$ac_top_builddir$srcdir;
    ;;
esac
dnl Don't blindly perform a `cd $1/$ac_foo && pwd` since $ac_foo can be
dnl absolute.

ac_buildpath=`cd $dstdir && cd $ac_builddir && pwd`
ac_top_buildpath=`cd $dstdir && cd $ac_top_builddir && pwd`
ac_srcpath=`cd $dstdir && cd $ac_srcdir && pwd`
ac_top_srcpath=`cd $dstdir && cd $ac_top_srcdir && pwd`
])# _AC_SRCPATHS

dnl _RTEMS_OUTPUT_SUBDIRS([host|target|build],[HOST|TARGET|BUILD])
AC_DEFUN([_RTEMS_OUTPUT_SUBDIRS],[
m4_ifdef([_RTEMS_$2_CONFIGDIRS_LIST],
[
if test "$no_recursion" != yes; then

 if test -n "${$1_SUBDIRS}"; then
   ac_sub_configure_args="[$]$1args"

  # Always prepend --prefix to ensure using the same prefix
  # in subdir configurations.
  ac_sub_configure_args="'--prefix=$prefix' $ac_sub_configure_args"

  # make sure that $1_subdir is not empty
  test -n "$$1_subdir" || $1_subdir="."

  case "$$1_subdir" in
  "." ) ;;
  * )
    ac_sub_configure_args="$ac_sub_configure_args '--with-target-subdir=$$1_subdir'"
    ac_sub_configure_args="$ac_sub_configure_args '--exec-prefix=${prefix}/$$1_subdir'"
    ac_sub_configure_args="$ac_sub_configure_args '--includedir=${prefix}/$$1_subdir/include'"
    ;;
  esac

  ac_popdir=`pwd`
  for ac_dir in : $$1_configdirs; do test "x$ac_dir" = x: && continue

    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    test -d $srcdir/$ac_dir || continue

    AC_MSG_NOTICE([configuring in $$1_subdir/$ac_dir])
    AS_MKDIR_P(["$$1_subdir/$ac_dir"])
    _RTEMS_SRCPATHS(["$ac_dir"],["$$1_subdir"])

    cd $$1_subdir/$ac_dir

    # Check for guested configure; otherwise get Cygnus style configure.
    if test -f $ac_srcdir/configure.gnu; then
      ac_sub_configure="$SHELL '$ac_srcdir/configure.gnu'"
    elif test -f $ac_srcdir/configure; then
      ac_sub_configure="$SHELL '$ac_srcdir/configure'"
    elif test -f $ac_srcdir/configure.in; then
      ac_sub_configure=$ac_configure
    else
      AC_MSG_WARN([no configuration information is in $ac_dir])
      ac_sub_configure=
    fi

    # The recursion is here.
    if test -n "$ac_sub_configure"; then
      # Make the cache file name correct relative to the subdirectory.
      case $cache_file in
      [[\\/]]* | ?:[[\\/]]* ) ac_sub_cache_file=$cache_file ;;
      *) # Relative path.
        ac_sub_cache_file=$ac_top_builddir$cache_file ;;
      esac

      ac_sub_configure_vars=
      ac_sub_configure_vars="$ac_sub_configure_vars '--cache-file=$ac_sub_cache_file'"
      ac_sub_configure_vars="$ac_sub_configure_vars '--srcdir=$ac_srcdir'"

      test -n "[$]CC_FOR_$2" && \
	ac_sub_configure_vars="$ac_sub_configure_vars 'CC=[$]CC_FOR_$2'"
      test -n "[$]CXX_FOR_$2" && \
        ac_sub_configure_vars="$ac_sub_configure_vars 'CXX=[$]CXX_FOR_$2'"
      test -n "[$]CFLAGS_FOR_$2" && \
        ac_sub_configure_vars="$ac_sub_configure_vars 'CFLAGS=[$]CFLAGS_FOR_$2'"
      test -n "[$]CXXFLAGS_FOR_$2" && \
        ac_sub_configure_vars="$ac_sub_configure_vars 'CXXFLAGS=[$]CXXFLAGS_FOR_$2'"

      # The eval makes quoting arguments work.
      AC_MSG_NOTICE([running $ac_sub_configure $ac_sub_configure_args $ac_sub_configure_vars])
      eval $ac_sub_configure $ac_sub_configure_args $ac_sub_configure_vars ||
        AC_MSG_ERROR([$ac_sub_configure failed for $ac_dir])
    fi

    cd "$ac_popdir"
  done
 fi
fi
])
])

AC_DEFUN([_RTEMS_CONFIGURE_ARGS_PRUNE],
[
$1_prune()
{
  $1=
  for ac_arg
  do
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

    -host* | --host* );;
    -host | --host ) 
      ac_prev=host_alias;;
    -target* | --target* );;
    -target | --target )
      ac_prev=target_alias;;
    -build* | --build* );;
    -build | --build ) 
      ac_prev=build_alias;;

    -prefix | --prefix | --prefi | --pref | --pre | --pr | --p)
      ac_prev=prefix ;;
    -prefix=* | --prefix=* | --prefi=* | --pref=* | --pre=* | --pr=* | --p=*)
      ;;

    *_alias=* );;

    m4_if([$2],,,[$2])
    *) $1="$$1 '$ac_arg'" ;;
   esac
  done
  export $1
}
])

## PUBLIC: RTEMS_BUILD_CONFIG_SUBDIRS(build_subdir)
# subdirs to be built for the build environment
AC_DEFUN([RTEMS_BUILD_CONFIG_SUBDIRS],[
m4_append([_RTEMS_BUILD_CONFIGDIRS_LIST],[ $1])
dnl Always append to build_configdirs
AC_SUBST(build_configdirs,"$build_configdirs $1")

m4_divert_text([DEFAULTS],
               [ac_subdirs_all="$ac_subdirs_all m4_normalize([$1])"])
m4_expand_once([_RTEMS_COMMANDS_POST_CONFIG_SUBDIRS])
])

AC_DEFUN([_RTEMS_BUILD_CONFIG_PREPARE],[
## # Record the configure arguments in Makefile.
m4_ifdef([_RTEMS_BUILD_CONFIGDIRS_LIST],
[
m4_expand_once([_RTEMS_TOOLS([build],[BUILD])])
m4_expand_once([_RTEMS_CONFIGURE_ARGS_PRUNE([buildargs])])
eval buildargs_prune $ac_configure_args
buildargs="'--host=${build_alias-$build}' '--build=${build_alias-$build}' ${buildargs}"
buildargs="${buildargs} '--target=${target_alias-$target}'"
],[])
AC_SUBST(buildargs)
AC_SUBST(build_subdir)
])

## PUBLIC: RTEMS_HOST_CONFIG_SUBDIRS(host_subdir)
# subdirs to be build for the host environment
AC_DEFUN([RTEMS_HOST_CONFIG_SUBDIRS],[
m4_append([_RTEMS_HOST_CONFIGDIRS_LIST],[ $1])dnl

_RTEMS_SUBST_IFNOT([host_configdirs],[$1])

m4_divert_text([DEFAULTS],
               [ac_subdirs_all="$ac_subdirs_all m4_normalize([$1])"])
m4_expand_once([_RTEMS_COMMANDS_POST_CONFIG_SUBDIRS])
])

AC_DEFUN([_RTEMS_HOST_CONFIG_PREPARE],[
m4_ifdef([_RTEMS_HOST_CONFIGDIRS_LIST],
[
m4_expand_once([_RTEMS_TOOLS([host],[HOST])])
m4_expand_once([_RTEMS_CONFIGURE_ARGS_PRUNE([hostargs])])
eval hostargs_prune $ac_configure_args
hostargs="'--host=${host_alias-$host}' '--build=${build_alias-$build}' '--target=${target_alias-$target}' ${hostargs}"
],[])
AC_SUBST(hostargs)
AC_SUBST(host_subdir)
])

## PUBLIC: RTEMS_TARGET_CONFIG_SUBDIRS(target_subdir)
# subdirs to be build for the target environment
AC_DEFUN([RTEMS_TARGET_CONFIG_SUBDIRS],[
AS_IF([test -d ${srcdir}/$1],[
m4_append([_RTEMS_TARGET_CONFIGDIRS_LIST],[ $1])

_RTEMS_SUBST_IFNOT([target_configdirs],[$1])

m4_divert_text([DEFAULTS],
               [ac_subdirs_all="$ac_subdirs_all m4_normalize([$1])"])
m4_expand_once([_RTEMS_COMMANDS_POST_CONFIG_SUBDIRS])
  ])
])

AC_DEFUN([_RTEMS_TARGET_CONFIG_PREPARE],[
m4_ifdef([_RTEMS_TARGET_CONFIGDIRS_LIST],
[
m4_expand_once([_RTEMS_TOOLS([target],[TARGET])])
m4_expand_once([_RTEMS_CONFIGURE_ARGS_PRUNE([targetargs])])
eval targetargs_prune $ac_configure_args
targetargs="'--host=${target_alias-$target}' '--build=${build_alias-$build}' '--target=${target_alias-$target}' ${targetargs}"
],[])
AC_SUBST(targetargs)
AC_SUBST(target_subdir)
])
