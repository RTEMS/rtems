dnl aclocal.m4 generated automatically by aclocal 1.4a

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

dnl $Id$

dnl
dnl RTEMS_TOP($1)
dnl 
dnl $1 .. relative path from this configure.in to the toplevel configure.in
dnl
AC_DEFUN(RTEMS_TOP,
[dnl
AC_ARG_WITH(target-subdir,
[  --with-target-subdir=DIR],
TARGET_SUBDIR="$withval",
TARGET_SUBDIR=".")

RTEMS_TOPdir="$1";
AC_SUBST(RTEMS_TOPdir)

dnl Determine RTEMS Version string from the VERSION file
dnl Hopefully, Joel never changes its format ;-
AC_MSG_CHECKING([for RTEMS Version])
if test -r "${srcdir}/${RTEMS_TOPdir}/VERSION"; then
changequote(,)dnl
RTEMS_VERSION=`grep Version ${srcdir}/${RTEMS_TOPdir}/VERSION | \
sed -e 's%RTEMS[ 	]*Version[ 	]*\(.*\)[ 	]*%\1%g'`
changequote([,])dnl
else
AC_MSG_ERROR(Unable to find ${RTEMS_TOPdir}/VERSION)
fi
if test -z "$RTEMS_VERSION"; then
AC_MSG_ERROR(Unable to determine version)
fi
AC_MSG_RESULT($RTEMS_VERSION)
])dnl

dnl
dnl $Id$
dnl

dnl canonicalize target cpu
dnl NOTE: Most rtems targets do not fullfil autoconf's
dnl target naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN(RTEMS_CANONICAL_TARGET_CPU,
[
AC_CANONICAL_SYSTEM
AC_MSG_CHECKING(rtems target cpu)
changequote(,)dnl
case "${target}" in
  # hpux unix port should go here
  i[3456]86-go32-rtems*)
	RTEMS_CPU=i386
	;;
  i[3456]86-pc-linux*)		# unix "simulator" port
	RTEMS_CPU=unix
	;;
  i[3456]86-*freebsd2*) 	# unix "simulator" port
	RTEMS_CPU=unix
	;;
  no_cpu-*rtems*)
        RTEMS_CPU=no_cpu
	;;
  sparc-sun-solaris*)           # unix "simulator" port
	RTEMS_CPU=unix
	;;
  *) 
	RTEMS_CPU=`echo $target | sed 's%^\([^-]*\)-\(.*\)$%\1%'`
	;;
esac
changequote([,])dnl
AC_SUBST(RTEMS_CPU)
AC_MSG_RESULT($RTEMS_CPU)
])

# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AC_PROG_INSTALL])
dnl We require 2.13 because we rely on SHELL being computed by configure.
AC_PREREQ([2.13])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package]))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

# Add --enable-maintainer-mode option to configure.
# From Jim Meyering

# serial 1

AC_DEFUN(AM_MAINTAINER_MODE,
[AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
  dnl maintainer-mode is disabled by default
  AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode enable make rules and dependencies not useful
                          (and sometimes confusing) to the casual installer],
      USE_MAINTAINER_MODE=$enableval,
      USE_MAINTAINER_MODE=no)
  AC_MSG_RESULT($USE_MAINTAINER_MODE)
  AM_CONDITIONAL(MAINTAINER_MODE, test $USE_MAINTAINER_MODE = yes)
  MAINT=$MAINTAINER_MODE_TRUE
  AC_SUBST(MAINT)dnl
]
)

# Define a conditional.

AC_DEFUN(AM_CONDITIONAL,
[AC_SUBST($1_TRUE)
AC_SUBST($1_FALSE)
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_MULTIPROCESSING,
[
AC_ARG_ENABLE(multiprocessing,
[  --enable-multiprocessing             enable multiprocessing interface],
[case "${enableval}" in 
  yes) RTEMS_HAS_MULTIPROCESSING=yes ;;
  no) RTEMS_HAS_MULTIPROCESSING=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-multiprocessing option) ;;
esac],[RTEMS_HAS_MULTIPROCESSING=no])
AC_SUBST(RTEMS_HAS_MULTIPROCESSING)dnl
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_POSIX,
[
AC_ARG_ENABLE(posix,
[  --enable-posix                       enable posix interface],
[case "${enableval}" in 
  yes) RTEMS_HAS_POSIX_API=yes ;;
  no) RTEMS_HAS_POSIX_API=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-posix option) ;;
esac],[RTEMS_HAS_POSIX_API=yes]) 
AC_SUBST(RTEMS_HAS_POSIX_API)

changequote(,)dnl
case "${target}" in
  # hpux unix port should go here
  i[3456]86-go32-rtems*)
	RTEMS_HAS_POSIX_API=no
	;;
  i[3456]86-pc-linux*)         # unix "simulator" port
	RTEMS_HAS_POSIX_API=no
	;;
  i[3456]86-*freebsd2*) # unix "simulator" port
	RTEMS_HAS_POSIX_API=no
	;;
  no_cpu-*rtems*)
	RTEMS_HAS_POSIX_API=no
	;;
  sparc-sun-solaris*)             # unix "simulator" port
	RTEMS_HAS_POSIX_API=no
	;;
  *) 
	;;
esac
changequote([,])dnl
AC_SUBST(RTEMS_HAS_POSIX_API)
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_NETWORKING,
[
AC_ARG_ENABLE(networking,
[  --enable-networking                  enable TCP/IP stack],
[case "${enableval}" in
  yes) RTEMS_HAS_NETWORKING=yes ;;
  no) RTEMS_HAS_NETWORKING=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-networking option) ;;
esac],[RTEMS_HAS_NETWORKING=yes])
AC_SUBST(RTEMS_HAS_NETWORKING)dnl
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_RDBG,
[
AC_ARG_ENABLE(rdbg,
[  --enable-rdbg                        enable remote debugger],
[case "${enableval}" in
  yes) RTEMS_HAS_RDBG=yes ;;
  no) RTEMS_HAS_RDBG=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-rdbg option) ;;
esac],[RTEMS_HAS_RDBG=no])
AC_SUBST(RTEMS_HAS_RDBG)dnl
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_INLINES,
[AC_ARG_ENABLE(rtems-inlines,
[  --enable-rtems-inlines               enable RTEMS inline functions]
[                                       (default:enabled, disable to use macros)],
[case "${enableval}" in
  yes) RTEMS_USE_MACROS=no ;;
  no) RTEMS_USE_MACROS=yes ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for disable-rtems-inlines option) ;;
esac],[RTEMS_USE_MACROS=no])
AC_SUBST(RTEMS_USE_MACROS)dnl
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_CXX,
[
AC_ARG_ENABLE(cxx,
[  --enable-cxx                         enable C++ support,]
[                                       and build the rtems++ library],
[case "${enableval}" in
  yes) RTEMS_HAS_CPLUSPLUS=yes ;;
  no) RTEMS_HAS_CPLUSPLUS=no   ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-cxx option) ;;
esac], [RTEMS_HAS_CPLUSPLUS=no])
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_GCC28,
[
AC_ARG_ENABLE(gcc28,
[  --enable-gcc28                       enable use of gcc 2.8.x features],
[case "${enableval}" in
  yes) RTEMS_USE_GCC272=no ;;
  no) RTEMS_USE_GCC272=yes ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for gcc-28 option) ;;
esac],[RTEMS_USE_GCC272=no])
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_LIBCDIR,
[
AC_ARG_ENABLE(libcdir,
[  --enable-libcdir=directory           set the directory for the C library],
[ RTEMS_LIBC_DIR="${enableval}" ; \
test -d ${enableval} || AC_MSG_ERROR("$enableval is not a directory" ) ] )
AC_SUBST(RTEMS_LIBC_DIR)dnl
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_TESTS,
[
# If the tests are enabled, then find all the test suite Makefiles
AC_MSG_CHECKING([if the test suites are enabled? ])
AC_ARG_ENABLE(tests,
[  --enable-tests                       enable tests (default:disabled)],
  [case "${enableval}" in
    yes) tests_enabled=yes ;;
    no)  tests_enabled=no ;;
    *)   AC_MSG_ERROR(bad value ${enableval} for tests option) ;;
  esac], [tests_enabled=no])
AC_MSG_RESULT([$tests_enabled])
])

dnl $Id$
dnl
dnl FIXME: this needs to be reworked

AC_DEFUN(RTEMS_ENABLE_HWAPI,
[dnl
AC_ARG_ENABLE(hwapi, \
[  --enable-hwapi                       enable hardware API library],
[case "${enableval}" in
    yes) RTEMS_HAS_HWAPI=yes ;;
    no)  RTEMS_HAS_HWAPI=no ;;
    *)  AC_MSG_ERROR(bad value ${enableval} for hwapi option) ;;
  esac],[RTEMS_HAS_HWAPI=no])
AC_SUBST(RTEMS_HAS_HWAPI)dnl
])dnl

dnl $Id$

dnl check if RTEMS support a cpu
AC_DEFUN(RTEMS_CHECK_CPU,
[dnl
AC_REQUIRE([RTEMS_TOP])
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])
# Is this a supported CPU?
AC_MSG_CHECKING([if cpu $RTEMS_CPU is supported])
if test -d "$srcdir/$RTEMS_TOPdir/c/src/exec/score/cpu/$RTEMS_CPU"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_ERROR(no)
fi
])dnl


dnl $Id$

AC_DEFUN(RTEMS_CANONICAL_HOST,
[dnl
AC_REQUIRE([AC_CANONICAL_HOST])
RTEMS_HOST=$host_os
changequote(,)dnl
case "${target}" in
  # hpux unix port should go here
  i[3456]86-pc-linux*)         # unix "simulator" port
        RTEMS_HOST=Linux
	;;
  i[3456]86-*freebsd2*) # unix "simulator" port
        RTEMS_HOST=FreeBSD
	;;
  sparc-sun-solaris*)             # unix "simulator" port
        RTEMS_HOST=Solaris
	;;
  *) 
	;;
esac
changequote([,])dnl
AC_SUBST(RTEMS_HOST)
])dnl

dnl $Id$

AC_DEFUN(RTEMS_CONFIG_SUBDIRS,
[
define([RTEMS_TGT_SUBDIRS], 
ifdef([RTEMS_TGT_SUBDIRS], [RTEMS_TGT_SUBDIRS ],)[$1])dnl
target_subdirs="RTEMS_TGT_SUBDIRS"
AC_SUBST(target_subdirs)
])

dnl This is a subroutine of AC_OUTPUT.
dnl It is called after running config.status.
dnl AC_OUTPUT_SUBDIRS(DIRECTORY...)
AC_DEFUN(RTEMS_OUTPUT_SUBDIRS,
[
if test "$no_recursion" != yes; then
  if test $target_alias != $host_alias; then 
    target_subdir="$target_alias"
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
    *) ac_sub_configure_args="$ac_sub_configure_args $ac_arg" ;;
    esac
  done

  test -d $target_subdir || mkdir $target_subdir
  for ac_config_dir in $1; do

    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    if test ! -d $srcdir/$ac_config_dir; then
      continue
    fi

    echo configuring in $target_subdir/$ac_config_dir

    case "$srcdir" in
    .) ;;
    *)
      if test -d $target_subdir/$ac_config_dir || mkdir $target_subdir/$ac_config_dir; then :;
      else
        AC_MSG_ERROR(can not create `pwd`/$target_subdir/$ac_config_dir)
      fi
      ;;
    esac

    ac_popdir=`pwd`
    cd $target_subdir/$ac_config_dir

changequote(, )dnl
      # A "../" for each directory in /$ac_config_dir.
      ac_dots=`echo $target_subdir/$ac_config_dir|sed -e 's%^\./%%' -e 's%[^/]$%&/%' -e 's%[^/]*/%../%g'`
changequote([, ])dnl

    case "$srcdir" in
    .) # No --srcdir option.  We are building in place.
      ac_sub_srcdir=$srcdir ;;
    /*) # Absolute path.
      ac_sub_srcdir=$srcdir/$ac_config_dir ;;
    *) # Relative path.
      ac_sub_srcdir=$ac_dots$srcdir/$ac_config_dir ;;
    esac

    # Check for guested configure; otherwise get Cygnus style configure.
    if test -f $ac_sub_srcdir/configure; then
      ac_sub_configure=$ac_sub_srcdir/configure
    elif test -f $ac_sub_srcdir/configure.in; then
      ac_sub_configure=$ac_configure
    else
      AC_MSG_WARN(no configuration information is in $ac_config_dir)
      ac_sub_configure=
    fi

    # The recursion is here.
    if test -n "$ac_sub_configure"; then

      # Make the cache file name correct relative to the subdirectory.
      if test "$target_alias" != "$host_alias"; then
      ac_sub_cache_file=$cache_file
      else
      case "$cache_file" in
      /*) ac_sub_cache_file=$cache_file ;;
      *) # Relative path.
        ac_sub_cache_file="$ac_dots$cache_file" ;;
      esac
      fi
ifdef([AC_PROVIDE_AC_PROG_INSTALL],
      [  case "$ac_given_INSTALL" in
changequote(, )dnl
        [/$]*) INSTALL="$ac_given_INSTALL" ;;
changequote([, ])dnl
        *) INSTALL="$ac_dots$ac_given_INSTALL" ;;
        esac
])dnl

      echo "[running ${CONFIG_SHELL-/bin/sh} $ac_sub_configure $ac_sub_configure_args --cache-file=$ac_sub_cache_file] --srcdir=$ac_sub_srcdir"
      # The eval makes quoting arguments work.
      if eval ${CONFIG_SHELL-/bin/sh} $ac_sub_configure \
	$ac_sub_configure_args --srcdir=$ac_sub_srcdir \
	--with-target-subdir=$target_subdir --cache-file=$ac_sub_cache_file 
      then :
      else
        AC_MSG_ERROR($ac_sub_configure failed for $ac_config_dir)
      fi
    fi

    cd $ac_popdir
  done
fi
])

