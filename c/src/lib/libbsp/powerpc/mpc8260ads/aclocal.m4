# aclocal.m4 generated automatically by aclocal 1.5

# Copyright 1996, 1997, 1998, 1999, 2000, 2001
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

dnl $Id$

dnl
dnl RTEMS_TOP($1)
dnl 
dnl $1 .. relative path from this configure.in to the toplevel configure.in
dnl
AC_DEFUN(RTEMS_TOP,
[dnl
AC_CHECK_PROGS(MAKE, gmake make)
AC_BEFORE([$0], [AC_CONFIG_AUX_DIR])dnl
AC_BEFORE([$0], [AM_INIT_AUTOMAKE])dnl

AC_PREFIX_DEFAULT([/opt/rtems])

ENDIF=endif
AC_SUBST(ENDIF)

RTEMS_TOPdir="$1";
AC_SUBST(RTEMS_TOPdir)

test -n "$with_target_subdir" || with_target_subdir="."

if test "$with_target_subdir" = "." ; then
# Native
PROJECT_TOPdir=${RTEMS_TOPdir}/'$(top_builddir)'
else
# Cross
dots=`echo $with_target_subdir|\
sed -e 's%^\./%%' -e 's%[[^/]]$%&/%' -e 's%[[^/]]*/%../%g'`
PROJECT_TOPdir=${dots}${RTEMS_TOPdir}/'$(top_builddir)'
fi
AC_SUBST(PROJECT_TOPdir)

if test "$with_target_subdir" = "." ; then
# Native
PROJECT_ROOT=${RTEMS_TOPdir}/'$(top_builddir)';
else
# Cross
PROJECT_ROOT=${RTEMS_TOPdir}/'$(top_builddir)'
fi
AC_SUBST(PROJECT_ROOT)

dnl Determine RTEMS Version string from the VERSION file
dnl Hopefully, Joel never changes its format ;-
AC_MSG_CHECKING([for RTEMS Version])
if test -r "${srcdir}/${RTEMS_TOPdir}/VERSION"; then
RTEMS_VERSION=`grep 'RTEMS Version' ${srcdir}/${RTEMS_TOPdir}/VERSION | \
sed -e 's%RTEMS[[ 	]]*Version[[ 	]]*\(.*\)[[ 	]]*%\1%g'`
else
AC_MSG_ERROR(Unable to find ${RTEMS_TOPdir}/VERSION)
fi
if test -z "$RTEMS_VERSION"; then
AC_MSG_ERROR(Unable to determine version)
fi
AC_MSG_RESULT($RTEMS_VERSION)
])dnl

# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 5

# There are a few dirty hacks below to avoid letting `AC_PROG_CC' be
# written in clear, in which case automake, when reading aclocal.m4,
# will think it sees a *use*, and therefore will trigger all it's
# C support machinery.  Also note that it means that autoscan, seeing
# CC etc. in the Makefile, will ask for an AC_PROG_CC use...


# We require 2.13 because we rely on SHELL being computed by configure.
AC_PREREQ([2.13])

# AC_PROVIDE_IFELSE(MACRO-NAME, IF-PROVIDED, IF-NOT-PROVIDED)
# -----------------------------------------------------------
# If MACRO-NAME is provided do IF-PROVIDED, else IF-NOT-PROVIDED.
# The purpose of this macro is to provide the user with a means to
# check macros which are provided without letting her know how the
# information is coded.
# If this macro is not defined by Autoconf, define it here.
ifdef([AC_PROVIDE_IFELSE],
      [],
      [define([AC_PROVIDE_IFELSE],
              [ifdef([AC_PROVIDE_$1],
                     [$2], [$3])])])


# AM_INIT_AUTOMAKE(PACKAGE,VERSION, [NO-DEFINE])
# ----------------------------------------------
AC_DEFUN([AM_INIT_AUTOMAKE],
[AC_REQUIRE([AC_PROG_INSTALL])dnl
# test to see if srcdir already configured
if test "`CDPATH=:; cd $srcdir && pwd`" != "`pwd`" &&
   test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run \"make distclean\" there first])
fi

# Define the identity of the package.
PACKAGE=$1
AC_SUBST(PACKAGE)dnl
VERSION=$2
AC_SUBST(VERSION)dnl
ifelse([$3],,
[AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package])])

# Autoconf 2.50 wants to disallow AM_ names.  We explicitly allow
# the ones we care about.
ifdef([m4_pattern_allow],
      [m4_pattern_allow([^AM_[A-Z]+FLAGS])])dnl

# Autoconf 2.50 always computes EXEEXT.  However we need to be
# compatible with 2.13, for now.  So we always define EXEEXT, but we
# don't compute it.
AC_SUBST(EXEEXT)
# Similar for OBJEXT -- only we only use OBJEXT if the user actually
# requests that it be used.  This is a bit dumb.
: ${OBJEXT=o}
AC_SUBST(OBJEXT)

# Some tools Automake needs.
AC_REQUIRE([AM_SANITY_CHECK])dnl
AC_REQUIRE([AC_ARG_PROGRAM])dnl
AM_MISSING_PROG(ACLOCAL, aclocal)
AM_MISSING_PROG(AUTOCONF, autoconf)
AM_MISSING_PROG(AUTOMAKE, automake)
AM_MISSING_PROG(AUTOHEADER, autoheader)
AM_MISSING_PROG(MAKEINFO, makeinfo)
AM_MISSING_PROG(AMTAR, tar)
AM_PROG_INSTALL_SH
AM_PROG_INSTALL_STRIP
# We need awk for the "check" target.  The system "awk" is bad on
# some platforms.
AC_REQUIRE([AC_PROG_AWK])dnl
AC_REQUIRE([AC_PROG_MAKE_SET])dnl
AC_REQUIRE([AM_DEP_TRACK])dnl
AC_REQUIRE([AM_SET_DEPDIR])dnl
AC_PROVIDE_IFELSE([AC_PROG_][CC],
                  [_AM_DEPENDENCIES(CC)],
                  [define([AC_PROG_][CC],
                          defn([AC_PROG_][CC])[_AM_DEPENDENCIES(CC)])])dnl
AC_PROVIDE_IFELSE([AC_PROG_][CXX],
                  [_AM_DEPENDENCIES(CXX)],
                  [define([AC_PROG_][CXX],
                          defn([AC_PROG_][CXX])[_AM_DEPENDENCIES(CXX)])])dnl
])

#
# Check to make sure that the build environment is sane.
#

# serial 3

# AM_SANITY_CHECK
# ---------------
AC_DEFUN([AM_SANITY_CHECK],
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftest.file
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftest.file 2> /dev/null`
   if test "$[*]" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftest.file`
   fi
   rm -f conftest.file
   if test "$[*]" != "X $srcdir/configure conftest.file" \
      && test "$[*]" != "X conftest.file $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "$[2]" = conftest.file
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
AC_MSG_RESULT(yes)])


# serial 2

# AM_MISSING_PROG(NAME, PROGRAM)
# ------------------------------
AC_DEFUN([AM_MISSING_PROG],
[AC_REQUIRE([AM_MISSING_HAS_RUN])
$1=${$1-"${am_missing_run}$2"}
AC_SUBST($1)])


# AM_MISSING_HAS_RUN
# ------------------
# Define MISSING if not defined so far and test if it supports --run.
# If it does, set am_missing_run to use it, otherwise, to nothing.
AC_DEFUN([AM_MISSING_HAS_RUN],
[AC_REQUIRE([AM_AUX_DIR_EXPAND])dnl
test x"${MISSING+set}" = xset || MISSING="\${SHELL} $am_aux_dir/missing"
# Use eval to expand $SHELL
if eval "$MISSING --run true"; then
  am_missing_run="$MISSING --run "
else
  am_missing_run=
  am_backtick='`'
  AC_MSG_WARN([${am_backtick}missing' script is too old or missing])
fi
])

# AM_AUX_DIR_EXPAND

# For projects using AC_CONFIG_AUX_DIR([foo]), Autoconf sets
# $ac_aux_dir to `$srcdir/foo'.  In other projects, it is set to
# `$srcdir', `$srcdir/..', or `$srcdir/../..'.
#
# Of course, Automake must honor this variable whenever it calls a
# tool from the auxiliary directory.  The problem is that $srcdir (and
# therefore $ac_aux_dir as well) can be either absolute or relative,
# depending on how configure is run.  This is pretty annoying, since
# it makes $ac_aux_dir quite unusable in subdirectories: in the top
# source directory, any form will work fine, but in subdirectories a
# relative path needs to be adjusted first.
#
# $ac_aux_dir/missing
#    fails when called from a subdirectory if $ac_aux_dir is relative
# $top_srcdir/$ac_aux_dir/missing
#    fails if $ac_aux_dir is absolute,
#    fails when called from a subdirectory in a VPATH build with
#          a relative $ac_aux_dir
#
# The reason of the latter failure is that $top_srcdir and $ac_aux_dir
# are both prefixed by $srcdir.  In an in-source build this is usually
# harmless because $srcdir is `.', but things will broke when you
# start a VPATH build or use an absolute $srcdir.
#
# So we could use something similar to $top_srcdir/$ac_aux_dir/missing,
# iff we strip the leading $srcdir from $ac_aux_dir.  That would be:
#   am_aux_dir='\$(top_srcdir)/'`expr "$ac_aux_dir" : "$srcdir//*\(.*\)"`
# and then we would define $MISSING as
#   MISSING="\${SHELL} $am_aux_dir/missing"
# This will work as long as MISSING is not called from configure, because
# unfortunately $(top_srcdir) has no meaning in configure.
# However there are other variables, like CC, which are often used in
# configure, and could therefore not use this "fixed" $ac_aux_dir.
#
# Another solution, used here, is to always expand $ac_aux_dir to an
# absolute PATH.  The drawback is that using absolute paths prevent a
# configured tree to be moved without reconfiguration.

AC_DEFUN([AM_AUX_DIR_EXPAND], [
# expand $ac_aux_dir to an absolute path
am_aux_dir=`CDPATH=:; cd $ac_aux_dir && pwd`
])

# AM_PROG_INSTALL_SH
# ------------------
# Define $install_sh.
AC_DEFUN([AM_PROG_INSTALL_SH],
[AC_REQUIRE([AM_AUX_DIR_EXPAND])dnl
install_sh=${install_sh-"$am_aux_dir/install-sh"}
AC_SUBST(install_sh)])

# One issue with vendor `install' (even GNU) is that you can't
# specify the program used to strip binaries.  This is especially
# annoying in cross-compiling environments, where the build's strip
# is unlikely to handle the host's binaries.
# Fortunately install-sh will honor a STRIPPROG variable, so we
# always use install-sh in `make install-strip', and initialize
# STRIPPROG with the value of the STRIP variable (set by the user).
AC_DEFUN([AM_PROG_INSTALL_STRIP],
[AC_REQUIRE([AM_PROG_INSTALL_SH])dnl
INSTALL_STRIP_PROGRAM="\${SHELL} \$(install_sh) -c -s"
AC_SUBST([INSTALL_STRIP_PROGRAM])])

# serial 4						-*- Autoconf -*-



# There are a few dirty hacks below to avoid letting `AC_PROG_CC' be
# written in clear, in which case automake, when reading aclocal.m4,
# will think it sees a *use*, and therefore will trigger all it's
# C support machinery.  Also note that it means that autoscan, seeing
# CC etc. in the Makefile, will ask for an AC_PROG_CC use...



# _AM_DEPENDENCIES(NAME)
# ---------------------
# See how the compiler implements dependency checking.
# NAME is "CC", "CXX" or "OBJC".
# We try a few techniques and use that to set a single cache variable.
#
# We don't AC_REQUIRE the corresponding AC_PROG_CC since the latter was
# modified to invoke _AM_DEPENDENCIES(CC); we would have a circular
# dependency, and given that the user is not expected to run this macro,
# just rely on AC_PROG_CC.
AC_DEFUN([_AM_DEPENDENCIES],
[AC_REQUIRE([AM_SET_DEPDIR])dnl
AC_REQUIRE([AM_OUTPUT_DEPENDENCY_COMMANDS])dnl
AC_REQUIRE([AM_MAKE_INCLUDE])dnl
AC_REQUIRE([AM_DEP_TRACK])dnl

ifelse([$1], CC,   [depcc="$CC"   am_compiler_list=],
       [$1], CXX,  [depcc="$CXX"  am_compiler_list=],
       [$1], OBJC, [depcc="$OBJC" am_compiler_list='gcc3 gcc']
       [$1], GCJ,  [depcc="$GCJ"  am_compiler_list='gcc3 gcc'],
                   [depcc="$$1"   am_compiler_list=])

AC_CACHE_CHECK([dependency style of $depcc],
               [am_cv_$1_dependencies_compiler_type],
[if test -z "$AMDEP_TRUE" && test -f "$am_depcomp"; then
  # We make a subdir and do the tests there.  Otherwise we can end up
  # making bogus files that we don't know about and never remove.  For
  # instance it was reported that on HP-UX the gcc test will end up
  # making a dummy file named `D' -- because `-MD' means `put the output
  # in D'.
  mkdir conftest.dir
  # Copy depcomp to subdir because otherwise we won't find it if we're
  # using a relative directory.
  cp "$am_depcomp" conftest.dir
  cd conftest.dir

  am_cv_$1_dependencies_compiler_type=none
  if test "$am_compiler_list" = ""; then
     am_compiler_list=`sed -n ['s/^#*\([a-zA-Z0-9]*\))$/\1/p'] < ./depcomp`
  fi
  for depmode in $am_compiler_list; do
    # We need to recreate these files for each test, as the compiler may
    # overwrite some of them when testing with obscure command lines.
    # This happens at least with the AIX C compiler.
    echo '#include "conftest.h"' > conftest.c
    echo 'int i;' > conftest.h
    echo "${am__include} ${am__quote}conftest.Po${am__quote}" > confmf

    case $depmode in
    nosideeffect)
      # after this tag, mechanisms are not by side-effect, so they'll
      # only be used when explicitly requested
      if test "x$enable_dependency_tracking" = xyes; then
	continue
      else
	break
      fi
      ;;
    none) break ;;
    esac
    # We check with `-c' and `-o' for the sake of the "dashmstdout"
    # mode.  It turns out that the SunPro C++ compiler does not properly
    # handle `-M -o', and we need to detect this.
    if depmode=$depmode \
       source=conftest.c object=conftest.o \
       depfile=conftest.Po tmpdepfile=conftest.TPo \
       $SHELL ./depcomp $depcc -c conftest.c -o conftest.o >/dev/null 2>&1 &&
       grep conftest.h conftest.Po > /dev/null 2>&1 &&
       ${MAKE-make} -s -f confmf > /dev/null 2>&1; then
      am_cv_$1_dependencies_compiler_type=$depmode
      break
    fi
  done

  cd ..
  rm -rf conftest.dir
else
  am_cv_$1_dependencies_compiler_type=none
fi
])
$1DEPMODE="depmode=$am_cv_$1_dependencies_compiler_type"
AC_SUBST([$1DEPMODE])
])


# AM_SET_DEPDIR
# -------------
# Choose a directory name for dependency files.
# This macro is AC_REQUIREd in _AM_DEPENDENCIES
AC_DEFUN([AM_SET_DEPDIR],
[rm -f .deps 2>/dev/null
mkdir .deps 2>/dev/null
if test -d .deps; then
  DEPDIR=.deps
else
  # MS-DOS does not allow filenames that begin with a dot.
  DEPDIR=_deps
fi
rmdir .deps 2>/dev/null
AC_SUBST(DEPDIR)
])


# AM_DEP_TRACK
# ------------
AC_DEFUN([AM_DEP_TRACK],
[AC_ARG_ENABLE(dependency-tracking,
[  --disable-dependency-tracking Speeds up one-time builds
  --enable-dependency-tracking  Do not reject slow dependency extractors])
if test "x$enable_dependency_tracking" != xno; then
  am_depcomp="$ac_aux_dir/depcomp"
  AMDEPBACKSLASH='\'
fi
AM_CONDITIONAL([AMDEP], [test "x$enable_dependency_tracking" != xno])
pushdef([subst], defn([AC_SUBST]))
subst(AMDEPBACKSLASH)
popdef([subst])
])

# Generate code to set up dependency tracking.
# This macro should only be invoked once -- use via AC_REQUIRE.
# Usage:
# AM_OUTPUT_DEPENDENCY_COMMANDS

#
# This code is only required when automatic dependency tracking
# is enabled.  FIXME.  This creates each `.P' file that we will
# need in order to bootstrap the dependency handling code.
AC_DEFUN([AM_OUTPUT_DEPENDENCY_COMMANDS],[
AC_OUTPUT_COMMANDS([
test x"$AMDEP_TRUE" != x"" ||
for mf in $CONFIG_FILES; do
  case "$mf" in
  Makefile) dirpart=.;;
  */Makefile) dirpart=`echo "$mf" | sed -e 's|/[^/]*$||'`;;
  *) continue;;
  esac
  grep '^DEP_FILES *= *[^ #]' < "$mf" > /dev/null || continue
  # Extract the definition of DEP_FILES from the Makefile without
  # running `make'.
  DEPDIR=`sed -n -e '/^DEPDIR = / s///p' < "$mf"`
  test -z "$DEPDIR" && continue
  # When using ansi2knr, U may be empty or an underscore; expand it
  U=`sed -n -e '/^U = / s///p' < "$mf"`
  test -d "$dirpart/$DEPDIR" || mkdir "$dirpart/$DEPDIR"
  # We invoke sed twice because it is the simplest approach to
  # changing $(DEPDIR) to its actual value in the expansion.
  for file in `sed -n -e '
    /^DEP_FILES = .*\\\\$/ {
      s/^DEP_FILES = //
      :loop
	s/\\\\$//
	p
	n
	/\\\\$/ b loop
      p
    }
    /^DEP_FILES = / s/^DEP_FILES = //p' < "$mf" | \
       sed -e 's/\$(DEPDIR)/'"$DEPDIR"'/g' -e 's/\$U/'"$U"'/g'`; do
    # Make sure the directory exists.
    test -f "$dirpart/$file" && continue
    fdir=`echo "$file" | sed -e 's|/[^/]*$||'`
    $ac_aux_dir/mkinstalldirs "$dirpart/$fdir" > /dev/null 2>&1
    # echo "creating $dirpart/$file"
    echo '# dummy' > "$dirpart/$file"
  done
done
], [AMDEP_TRUE="$AMDEP_TRUE"
ac_aux_dir="$ac_aux_dir"])])

# AM_MAKE_INCLUDE()
# -----------------
# Check to see how make treats includes.
AC_DEFUN([AM_MAKE_INCLUDE],
[am_make=${MAKE-make}
cat > confinc << 'END'
doit:
	@echo done
END
# If we don't find an include directive, just comment out the code.
AC_MSG_CHECKING([for style of include used by $am_make])
am__include='#'
am__quote=
_am_result=none
# First try GNU make style include.
echo "include confinc" > confmf
# We grep out `Entering directory' and `Leaving directory'
# messages which can occur if `w' ends up in MAKEFLAGS.
# In particular we don't look at `^make:' because GNU make might
# be invoked under some other name (usually "gmake"), in which
# case it prints its new name instead of `make'.
if test "`$am_make -s -f confmf 2> /dev/null | fgrep -v 'ing directory'`" = "done"; then
   am__include=include
   am__quote=
   _am_result=GNU
fi
# Now try BSD make style include.
if test "$am__include" = "#"; then
   echo '.include "confinc"' > confmf
   if test "`$am_make -s -f confmf 2> /dev/null`" = "done"; then
      am__include=.include
      am__quote='"'
      _am_result=BSD
   fi
fi
AC_SUBST(am__include)
AC_SUBST(am__quote)
AC_MSG_RESULT($_am_result)
rm -f confinc confmf
])

# serial 3

# AM_CONDITIONAL(NAME, SHELL-CONDITION)
# -------------------------------------
# Define a conditional.
#
# FIXME: Once using 2.50, use this:
# m4_match([$1], [^TRUE\|FALSE$], [AC_FATAL([$0: invalid condition: $1])])dnl
AC_DEFUN([AM_CONDITIONAL],
[ifelse([$1], [TRUE],
        [errprint(__file__:__line__: [$0: invalid condition: $1
])dnl
m4exit(1)])dnl
ifelse([$1], [FALSE],
       [errprint(__file__:__line__: [$0: invalid condition: $1
])dnl
m4exit(1)])dnl
AC_SUBST([$1_TRUE])
AC_SUBST([$1_FALSE])
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi])

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
AC_CANONICAL_TARGET
AC_MSG_CHECKING(rtems target cpu)
case "${target}" in
  # hpux unix port should go here
  i[[34567]]86-*linux*)		# unix "simulator" port
	RTEMS_CPU=unix
	;;
  i[[34567]]86-*freebsd*) 	# unix "simulator" port
	RTEMS_CPU=unix
	;;
  i[[34567]]86-pc-cygwin*) 	# Cygwin is just enough unix like :)
	RTEMS_CPU=unix
	;;
  no_cpu-*rtems*)
        RTEMS_CPU=no_cpu
	;;
  sparc-sun-solaris*)           # unix "simulator" port
	RTEMS_CPU=unix
	;;
  *) 
	RTEMS_CPU=`echo $target | sed 's%^\([[^-]]*\)-\(.*\)$%\1%'`
	;;
esac
AC_SUBST(RTEMS_CPU)
AC_MSG_RESULT($RTEMS_CPU)
])

# Add --enable-maintainer-mode option to configure.
# From Jim Meyering

# serial 1

AC_DEFUN([AM_MAINTAINER_MODE],
[AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
  dnl maintainer-mode is disabled by default
  AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode enable make rules and dependencies not useful
                          (and sometimes confusing) to the casual installer],
      USE_MAINTAINER_MODE=$enableval,
      USE_MAINTAINER_MODE=no)
  AC_MSG_RESULT([$USE_MAINTAINER_MODE])
  AM_CONDITIONAL(MAINTAINER_MODE, [test $USE_MAINTAINER_MODE = yes])
  MAINT=$MAINTAINER_MODE_TRUE
  AC_SUBST(MAINT)dnl
]
)

dnl
dnl $Id$
dnl 
dnl Check for target gcc
dnl

AC_DEFUN(RTEMS_PROG_CC,
[
AC_BEFORE([$0], [AC_PROG_CPP])dnl
AC_BEFORE([$0], [AC_PROG_CC])dnl
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl
AC_REQUIRE([RTEMS_ENABLE_LIBCDIR])dnl
AC_REQUIRE([RTEMS_ENABLE_GCC28])dnl

RTEMS_CHECK_TOOL(CC,gcc)
test -z "$CC" && \
  AC_MSG_ERROR([no acceptable cc found in \$PATH])
AC_PROG_CC
AC_PROG_CPP

AM_CONDITIONAL(RTEMS_USE_GCC,test x"$GCC" = x"yes")
])

AC_DEFUN(RTEMS_PROG_CC_FOR_TARGET,
[
dnl check target cc
RTEMS_PROG_CC
dnl check if the compiler supports --specs
RTEMS_GCC_SPECS
dnl check if the target compiler may use --pipe
RTEMS_GCC_PIPE
dnl check if the compiler supports --specs if gcc28 is requested
if test "$RTEMS_USE_GCC272" != "yes" ; then
  if test "$rtems_cv_gcc_specs" = "no"; then
    AC_MSG_WARN([*** disabling --enable-gcc28])
      RTEMS_USE_GCC272=yes
  fi
fi
test "$rtems_cv_gcc_pipe" = "yes" && CC="$CC --pipe"

if test "$GCC" = yes; then
]
ifelse([$1],,[],[CPPFLAGS="$CPPFLAGS $1"])
[
CFLAGS="-g -Wall"
fi
AM_CONDITIONAL(RTEMS_USE_GCC272, test x"$RTEMS_USE_GCC272" = x"yes")
AC_SUBST(RTEMS_USE_GCC272)

dnl FIXME: HACK for egcs/cygwin mixing '\\' and '/' in gcc -print-*
case $build_os in
*cygwin*)     GCCSED="| sed 's%\\\\%/%g'" ;;
*) ;;
esac
AC_SUBST(GCCSED)
])

dnl
dnl $Id$
dnl
dnl Set target tools
dnl

AC_DEFUN(RTEMS_GCC_PRINT,
[
    $1=`$CC --print-prog-name=$2`
])

AC_DEFUN(RTEMS_CANONICALIZE_TOOLS,
[AC_REQUIRE([RTEMS_PROG_CC])dnl

dnl FIXME: What shall be done if these tools are not available?
  RTEMS_CHECK_TOOL(AR,ar,no)
  RTEMS_CHECK_TOOL(AS,as,no)
  RTEMS_CHECK_TOOL(LD,ld,no)
  RTEMS_CHECK_TOOL(NM,nm,no)

dnl special treatment of ranlib
  RTEMS_CHECK_TOOL(RANLIB,ranlib,:)

dnl NOTE: These may not be available if not using gnutools
  RTEMS_CHECK_TOOL(OBJCOPY,objcopy,no)
  RTEMS_CHECK_TOOL(SIZE,size,no)
  RTEMS_CHECK_TOOL(STRIP,strip,:)
])

dnl $Id$

dnl RTEMS_CHECK_TOOL(VARIABLE, PROG-TO-CHECK-FOR[, VALUE-IF-NOT-FOUND [, PATH]])
AC_DEFUN(RTEMS_CHECK_TOOL,
[AC_REQUIRE([AC_CHECK_TOOL_PREFIX])dnl
AC_CHECK_PROG($1, ${ac_tool_prefix}$2, ${ac_tool_prefix}$2, $3, $4)
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_LIBCDIR,
[
AC_ARG_ENABLE(libcdir,
AC_HELP_STRING([--enable-libcdir=directory],
[set the directory for the C library]),
[ RTEMS_LIBC_DIR="${enableval}" ; \
test -d ${enableval} || AC_MSG_ERROR("$enableval is not a directory" ) ] )
AC_SUBST(RTEMS_LIBC_DIR)dnl
])


dnl $Id$

AC_DEFUN(RTEMS_ENABLE_GCC28,
[
AC_ARG_ENABLE(gcc28,
AC_HELP_STRING([--enable-gcc28],[enable use of gcc 2.8.x features]),
[case "${enableval}" in
  yes) RTEMS_USE_GCC272=no ;;
  no) RTEMS_USE_GCC272=yes ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for gcc-28 option) ;;
esac],[RTEMS_USE_GCC272=no])
])

dnl
dnl $Id$
dnl
dnl Check whether the target compiler accepts -specs
dnl

AC_DEFUN(RTEMS_GCC_SPECS,
[AC_REQUIRE([RTEMS_PROG_CC])
AC_CACHE_CHECK(whether $CC accepts -specs,rtems_cv_gcc_specs,
[
rtems_cv_gcc_specs=no
if test x"$GCC" = x"yes"; then
  touch confspec
  echo 'void f(){}' >conftest.c
  if test -z "`${CC} -specs confspec -c conftest.c 2>&1`";then
    rtems_cv_gcc_specs=yes
  fi
fi
rm -f confspec conftest*
])])

dnl
dnl $Id$
dnl
dnl Check whether the target compiler accepts -pipe
dnl

AC_DEFUN(RTEMS_GCC_PIPE,
[AC_REQUIRE([RTEMS_PROG_CC]) 
AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK(whether $CC accepts --pipe,rtems_cv_gcc_pipe,
[
rtems_cv_gcc_pipe=no
if test x"$GCC" = x"yes"; then
    echo 'void f(){}' >conftest.c
    if test -z "`${CC} --pipe -c conftest.c 2>&1`";then
      rtems_cv_gcc_pipe=yes
    fi
    rm -f conftest*
fi
])
])

dnl $Id$

AC_DEFUN(RTEMS_ENABLE_NETWORKING,
[

AC_ARG_ENABLE(networking,
AC_HELP_STRING([--enable-networking],[enable TCP/IP stack]),
[case "${enableval}" in
  yes) RTEMS_HAS_NETWORKING=yes ;;
  no) RTEMS_HAS_NETWORKING=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-networking option) ;;
esac],[RTEMS_HAS_NETWORKING=yes])
AC_SUBST(RTEMS_HAS_NETWORKING)dnl
])

dnl $Id$

dnl Pass a single BSP via an environment variable
dnl used by per BSP configure scripts
AC_DEFUN(RTEMS_ENV_RTEMSBSP,
[dnl
AC_BEFORE([$0], [RTEMS_ENABLE_RTEMSBSP])dnl
AC_BEFORE([$0], [RTEMS_PROJECT_ROOT])dnl
AC_BEFORE([$0], [RTEMS_CHECK_CUSTOM_BSP])dnl

AC_ARG_VAR([RTEMS_BSP],[RTEMS_BSP to build])
AC_MSG_CHECKING([for RTEMS_BSP])
AC_CACHE_VAL(rtems_cv_RTEMS_BSP,
[dnl
  test -n "${RTEMS_BSP}" && rtems_cv_RTEMS_BSP="$RTEMS_BSP";
])dnl
if test -z "$rtems_cv_RTEMS_BSP"; then
  AC_MSG_ERROR([Missing RTEMS_BSP])
fi
RTEMS_BSP="$rtems_cv_RTEMS_BSP"
AC_MSG_RESULT(${RTEMS_BSP})
AC_SUBST(RTEMS_BSP)

RTEMS_BSP_SPECS="-specs bsp_specs -qrtems"
AC_SUBST(RTEMS_BSP_SPECS)

GCC_SPECS="-B\$(PROJECT_ROOT)/ -B\$(PROJECT_ROOT)/$RTEMS_BSP/lib/"
AC_SUBST(GCC_SPECS)

RTEMS_ROOT=$PROJECT_ROOT/c/$RTEMS_BSP
AC_SUBST(RTEMS_ROOT)

RTEMS_ENABLE_BARE
AC_SUBST(BARE_CPU_MODEL)
AC_SUBST(BARE_CPU_CFLAGS)

AM_CONDITIONAL(RTEMS_CONFIG_PER_BSP, true);

includedir="\${exec_prefix}/\$(RTEMS_BSP)/lib/include"
])

dnl $Id$

dnl Override the set of BSPs to be built.
dnl used by the toplevel configure script
dnl RTEMS_ENABLE_RTEMSBSP(rtems_bsp_list)
AC_DEFUN(RTEMS_ENABLE_RTEMSBSP,
[
AC_BEFORE([$0], [RTEMS_ENV_RTEMSBSP])dnl
AC_ARG_ENABLE(rtemsbsp,
AC_HELP_STRING([--enable-rtemsbsp="bsp1 bsp2 .."],
[BSPs to include in build]),
[case "${enableval}" in
  yes|no) AC_MSG_ERROR([missing argument to --enable-rtemsbsp=\"bsp1 bsp2\"]);;
  *) $1=$enableval;;
esac],[$1=""])
])

dnl
dnl $Id$
dnl 

dnl
dnl PROJECT_TOPdir     .. relative path to the top of the build-tree
dnl PROJECT_ROOT       .. relative path to the top of the temporary
dnl                       installation directory inside the build-tree
dnl RTEMS_TOPdir       .. relative path of a subpackage's configure.in to the
dnl                       toplevel configure.in of the source-tree
dnl RTEMS_ROOT         .. path to the top of a bsp's build directory
dnl                       [Applied by custom/*.cfg, depredicated otherwise]
dnl

AC_DEFUN(RTEMS_PROJECT_ROOT,
[dnl
AC_REQUIRE([RTEMS_TOP])

PACKHEX="\$(PROJECT_TOPdir)/tools/build/packhex"
AC_SUBST(PACKHEX)

PROJECT_INCLUDE="\$(PROJECT_ROOT)/$RTEMS_BSP/lib/include"
AC_SUBST(PROJECT_INCLUDE)

PROJECT_RELEASE="\$(PROJECT_ROOT)/$RTEMS_BSP"
AC_SUBST(PROJECT_RELEASE)
])


dnl $Id$

dnl Report all available bsps for a target,
dnl check if a bsp-subdirectory is present for all bsps found
dnl
dnl RTEMS_CHECK_BSPS(bsp_list)
AC_DEFUN(RTEMS_CHECK_BSPS,
[
AC_REQUIRE([RTEMS_CHECK_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_TOP])dnl sets RTEMS_TOPdir
AC_MSG_CHECKING([for bsps])
    files=`ls $srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU`
    for file in $files; do
      case $file in
        ChangeLog*);;
        shared*);;
        Makefile*);;
        READ*);;
        CVS*);;
        pxfl*);;
        ac*);;
        config*);;
        # Now account for BSPs with build variants
        c4xsim)            rtems_bsp="$rtems_bsp c4xsim c3xsim";;
        gen68360)          rtems_bsp="$rtems_bsp gen68360 gen68360_040";;
        p4000)             rtems_bsp="$rtems_bsp p4600 p4650";;
        mvme162)           rtems_bsp="$rtems_bsp mvme162 mvme162lx";;
        mbx8xx)            rtems_bsp="$rtems_bsp mbx821_001 mbx860_002";;
	motorola_powerpc)  rtems_bsp="$rtems_bsp mvme2307 mcp750";;
	pc386)             rtems_bsp="$rtems_bsp pc386 pc386dx pc486 pc586 pc686 pck6";;
	erc32)             rtems_bsp="$rtems_bsp erc32 erc32nfp";;
	leon)              rtems_bsp="$rtems_bsp leon1 leon2";;
	sim68000)          rtems_bsp="$rtems_bsp sim68000 simcpu32";;
	shsim)             rtems_bsp="$rtems_bsp simsh7032 simsh7045";;
	*) if test -d $srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU/$file; then
	     $1="[$]$1 $file"
	   fi;;
      esac;
    done
AC_MSG_RESULT([[$]$1 .. done])
])dnl

AC_DEFUN(RTEMS_CHECK_CUSTOM_BSP,
[dnl
AC_REQUIRE([RTEMS_TOP])

AC_MSG_CHECKING([for make/custom/[$]$1.cfg])
if test -r "$srcdir/$RTEMS_TOPdir/make/custom/[$]$1.cfg"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_ERROR([no])
fi
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


AC_DEFUN(RTEMS_ENABLE_BARE,
[
AC_ARG_ENABLE(bare-cpu-cflags,
AC_HELP_STRING([--enable-bare-cpu-cflags],[specify a particular cpu cflag (bare bsp specific)]),
[case "${enableval}" in
  no) BARE_CPU_CFLAGS="" ;;
  *)    BARE_CPU_CFLAGS="${enableval}" ;;
esac],
[BARE_CPU_CFLAGS=""])

AC_ARG_ENABLE(bare-cpu-model,
AC_HELP_STRING([--enable-bare-cpu-model],[specify a particular cpu model (bare bsp specific)]),
[case "${enableval}" in
  no)   BARE_CPU_MODEL="" ;;
  *)    BARE_CPU_MODEL="${enableval}" ;;
esac],
[BARE_CPU_MODEL=""])
])


dnl $Id$

dnl RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)
AC_DEFUN(RTEMS_CHECK_BSP_CACHE,
[
AC_REQUIRE([RTEMS_CHECK_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_ENV_RTEMSBSP])dnl set RTEMS_BSP
AC_REQUIRE([RTEMS_TOP])dnl sets RTEMS_TOPdir
AC_CACHE_CHECK([for RTEMS_CPU_MODEL], rtems_cv_RTEMS_CPU_MODEL,
. $RTEMS_TOPdir/c/[$]$1/make/[$]$1.cache)
RTEMS_CPU_MODEL=$rtems_cv_RTEMS_CPU_MODEL
AC_SUBST(RTEMS_CPU_MODEL)
AC_CACHE_CHECK([for RTEMS_BSP_FAMILY], rtems_cv_RTEMS_BSP_FAMILY,
. $RTEMS_TOPdir/c/[$]$1/make/[$]$1.cache)
RTEMS_BSP_FAMILY=$rtems_cv_RTEMS_BSP_FAMILY
AC_SUBST(RTEMS_BSP_FAMILY)
])dnl

dnl $Id$
dnl
AC_DEFUN(RTEMS_CHECK_NETWORKING,
[dnl
AC_REQUIRE([RTEMS_CHECK_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_NETWORKING])dnl

AC_CACHE_CHECK([whether BSP supports networking],
  rtems_cv_HAS_NETWORKING,
  [dnl
    case "$RTEMS_CPU" in
    unix*)
      rtems_cv_HAS_NETWORKING="no"
      ;;
    *)
      if test "${RTEMS_HAS_NETWORKING}" = "yes"; then
        rtems_cv_HAS_NETWORKING="yes";
      else
        rtems_cv_HAS_NETWORKING="disabled";
      fi
      ;;
    esac])
if test "$rtems_cv_HAS_NETWORKING" = "yes"; then
  HAS_NETWORKING="yes";
else
  HAS_NETWORKING="no";
fi
AC_SUBST(HAS_NETWORKING)dnl
])

dnl $Id$

AC_DEFUN(RTEMS_CANONICAL_HOST,
[dnl
AC_REQUIRE([AC_CANONICAL_HOST])
RTEMS_HOST=$host_os
case "${target}" in
  # hpux unix port should go here
  i[[34567]]86-*linux*)          # unix "simulator" port
        RTEMS_HOST=Linux
	;;
  i[[34567]]86-*freebsd*)         # unix "simulator" port
        RTEMS_HOST=FreeBSD
	;;
  i[[34567]]86-pc-cygwin*) 	# Cygwin is just enough unix like :)
        RTEMS_HOST=Cygwin
	;;
  sparc-sun-solaris*)           # unix "simulator" port
        RTEMS_HOST=Solaris
	;;
  *) 
	;;
esac
AC_SUBST(RTEMS_HOST)
])dnl

