dnl aclocal.m4 generated automatically by aclocal 1.4

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

AC_DEFUN(RTEMS_ENABLE_BARE,
[
AC_ARG_ENABLE(bare-cpu-cflags,
[  --enable-bare-cpu-cflags             specify a particular cpu cflag]
[                                       (bare bsp specific)],
[case "${enableval}" in
  no) BARE_CPU_CFLAGS="" ;;
  *)    BARE_CPU_CFLAGS="${enableval}" ;;
esac],
[BARE_CPU_CFLAGS=""])

AC_ARG_ENABLE(bare-cpu-model,
[  --enable-bare-cpu-model              specify a particular cpu model]
[                                       (bare bsp specific)],
[case "${enableval}" in
  no)   BARE_CPU_MODEL="" ;;
  *)    BARE_CPU_MODEL="${enableval}" ;;
esac],
[BARE_CPU_MODEL=""])
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

dnl Override the set of BSPs to be built.
dnl used by the toplevel configure script
dnl RTEMS_ENABLE_RTEMSBSP(rtems_bsp_list)
AC_DEFUN(RTEMS_ENABLE_RTEMSBSP,
[
AC_ARG_ENABLE(rtemsbsp,
[  --enable-rtemsbsp=bsp1 bsp2 ..      BSPs to include in build],
[case "${enableval}" in
  yes|no) AC_MSG_ERROR([missing argument to --enable-rtemsbsp=\"bsp1 bsp2\"]);;
  *) $1=$enableval;;
esac],[$1=""])
])

dnl Pass a single BSP via an environment variable
dnl used by per BSP configure scripts
AC_DEFUN(RTEMS_ENV_RTEMSBSP,
[dnl
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
])

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

dnl
dnl $Id$
dnl 

AC_DEFUN(RTEMS_PROJECT_ROOT,
[dnl
AC_REQUIRE([RTEMS_TOP])
if test "$TARGET_SUBDIR" = "." ; then
PROJECT_ROOT=$RTEMS_TOPdir/'$(top_builddir)';
else
PROJECT_ROOT=../$RTEMS_TOPdir/'$(top_builddir)'
fi
AC_SUBST(PROJECT_ROOT)

RTEMS_ROOT=$RTEMS_TOPdir/'$(top_builddir)'/c/$RTEMS_BSP
AC_SUBST(RTEMS_ROOT)

INSTALL_CHANGE="\$(KSH) \$(PROJECT_ROOT)/tools/build/install-if-change"
AC_SUBST(INSTALL_CHANGE)

PACKHEX="\$(PROJECT_ROOT)/tools/build/packhex"
AC_SUBST(PACKHEX)
])


dnl
dnl $Id$
dnl 
dnl Check for target gcc
dnl
dnl 98/05/20 Ralf Corsepius 	(corsepiu@faw.uni-ulm.de)
dnl				Completely reworked

AC_DEFUN(RTEMS_PROG_CC,
[
AC_BEFORE([$0], [AC_PROG_CPP])dnl
AC_BEFORE([$0], [AC_PROG_CC])dnl
AC_REQUIRE([RTEMS_TOOL_PREFIX])dnl

dnl Only accept gcc and cc
dnl NOTE: This might be too restrictive for native compilation
AC_PATH_PROGS(CC_FOR_TARGET, "$program_prefix"gcc "$program_prefix"cc )
test -z "$CC_FOR_TARGET" \
  && AC_MSG_ERROR([no acceptable cc found in \$PATH])

dnl backup 
rtems_save_CC=$CC
rtems_save_CFLAGS=$CFLAGS

dnl temporarily set CC
CC=$CC_FOR_TARGET

AC_PROG_CC_WORKS
AC_PROG_CC_GNU

if test $ac_cv_prog_gcc = yes; then
  GCC=yes
dnl Check whether -g works, even if CFLAGS is set, in case the package
dnl plays around with CFLAGS (such as to build both debugging and
dnl normal versions of a library), tasteless as that idea is.
  ac_test_CFLAGS="${CFLAGS+set}"
  ac_save_CFLAGS="$CFLAGS"
  CFLAGS=
  AC_PROG_CC_G
  if test "$ac_test_CFLAGS" = set; then
    CFLAGS="$ac_save_CFLAGS"
  elif test $ac_cv_prog_cc_g = yes; then
    CFLAGS="-g -O2"
  else
    CFLAGS="-O2"
  fi
else
  GCC=
  test "${CFLAGS+set}" = set || CFLAGS="-g"
fi

rtems_cv_prog_gcc=$ac_cv_prog_gcc
rtems_cv_prog_cc_g=$ac_cv_prog_cc_g
rtems_cv_prog_cc_works=$ac_cv_prog_cc_works
rtems_cv_prog_cc_cross=$ac_cv_prog_cc_cross

dnl restore initial values
CC=$rtems_save_CC
CFLAGS=$rtems_save_CFLAGS

unset ac_cv_prog_gcc
unset ac_cv_prog_cc_g
unset ac_cv_prog_cc_works
unset ac_cv_prog_cc_cross
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
test "$rtems_cv_gcc_pipe" = "yes" && CC_FOR_TARGET="$CC_FOR_TARGET --pipe"

dnl FIXME: HACK for egcs/cygwin mixing '\\' and '/' in gcc -print-*
case $host_os in
*cygwin*)     GCCSED="| sed 's%\\\\%/%g'" ;;
*) ;;
esac
AC_SUBST(GCCSED)
])

dnl
dnl  $Id$
dnl 
dnl Set program_prefix
dnl
dnl 98/05/20 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl				Extracted from configure

AC_DEFUN(RTEMS_TOOL_PREFIX,
[AC_REQUIRE([AC_CANONICAL_TARGET])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl

changequote(,)dnl
if [ "${program_prefix}" = "NONE" ] ; then
  if [ "${target}" = "${host}" ] ; then
    program_prefix=
  else
    program_prefix=${target}-
  fi
fi
changequote([,])dnl
])

dnl
dnl $Id$
dnl
dnl Check whether the target compiler accepts -specs
dnl
dnl 98/02/11 Ralf Corsepius 	corsepiu@faw.uni-ulm.de
dnl

AC_DEFUN(RTEMS_GCC_SPECS,
[AC_REQUIRE([RTEMS_PROG_CC])
AC_CACHE_CHECK(whether $CC_FOR_TARGET accepts -specs,rtems_cv_gcc_specs,
[
rtems_cv_gcc_specs=no
if test "$rtems_cv_prog_gcc" = "yes"; then
  touch confspec
  echo 'void f(){}' >conftest.c
  if test -z "`${CC_FOR_TARGET} -specs confspec -c conftest.c 2>&1`";then
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
dnl 98/02/11 Ralf Corsepius     corsepiu@faw.uni-ulm.de
dnl

AC_DEFUN(RTEMS_GCC_PIPE,
[AC_REQUIRE([RTEMS_PROG_CC]) 
AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK(whether $CC_FOR_TARGET accepts --pipe,rtems_cv_gcc_pipe,
[
rtems_cv_gcc_pipe=no
if test "$rtems_cv_prog_gcc" = "yes"; then
case "$host_os" in
  cygwin*)
    ;;
  *)
    echo 'void f(){}' >conftest.c
    if test -z "`${CC_FOR_TARGET} --pipe -c conftest.c 2>&1`";then
      rtems_cv_gcc_pipe=yes
    fi
    rm -f conftest*
    ;;
esac
fi
])
])

dnl
dnl $Id$
dnl 
dnl Check for target g++
dnl 
dnl 98/05/20 Ralf Corsepius 	(corsepiu@faw.uni-ulm.de)
dnl				Completely reworked

AC_DEFUN(RTEMS_PROG_CXX,
[
AC_BEFORE([$0], [AC_PROG_CXXCPP])dnl
AC_BEFORE([$0], [AC_PROG_CXX])dnl
AC_REQUIRE([RTEMS_TOOL_PREFIX])dnl

dnl Only accept g++ and c++
dnl NOTE: This might be too restrictive for native compilation
AC_PATH_PROGS(CXX_FOR_TARGET, "$program_prefix"g++ "$program_prefix"c++)
test -z "$CXX_FOR_TARGET" \
  && AC_MSG_ERROR([no acceptable c++ found in \$PATH])

dnl backup 
rtems_save_CXX=$CXX
rtems_save_CXXFLAGS=$CXXFLAGS

dnl temporarily set CXX
CXX=$CXX_FOR_TARGET

AC_PROG_CXX_WORKS
AC_PROG_CXX_GNU

if test $ac_cv_prog_gxx = yes; then
  GXX=yes
dnl Check whether -g works, even if CXXFLAGS is set, in case the package
dnl plays around with CXXFLAGS (such as to build both debugging and
dnl normal versions of a library), tasteless as that idea is.
  ac_test_CXXFLAGS="${CXXFLAGS+set}"
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS=
  AC_PROG_CXX_G
  if test "$ac_test_CXXFLAGS" = set; then
    CXXFLAGS="$ac_save_CXXFLAGS"
  elif test $ac_cv_prog_cxx_g = yes; then
    CXXFLAGS="-g -O2"
  else
    CXXFLAGS="-O2"
  fi
else
  GXX=
  test "${CXXFLAGS+set}" = set || CXXFLAGS="-g"
fi

rtems_cv_prog_gxx=$ac_cv_prog_gxx
rtems_cv_prog_cxx_g=$ac_cv_prog_cxx_g
rtems_cv_prog_cxx_works=$ac_cv_prog_cxx_works
rtems_cv_prog_cxx_cross=$ac_cv_prog_cxx_cross

CXX=$rtems_save_CXX
CXXFLAGS=$rtems_save_CXXFLAGS

dnl restore initial values
unset ac_cv_prog_gxx
unset ac_cv_prog_cc_g
unset ac_cv_prog_cxx_works
unset ac_cv_prog_cxx_cross
])

AC_DEFUN(RTEMS_PROG_CXX_FOR_TARGET,
[
  RTEMS_PROG_CXX
  if test "$rtems_cv_prog_cc_cross" != "$rtems_cv_prog_cxx_cross"; then
    AC_MSG_ERROR([***]
     [Inconsistency in compiler configuration:]
     [Target C compiler and Target C++ compiler]
     [must both either be cross compilers or native compilers]
     [Hint: If building a posix bsp: LD_LIBRARY_PATH?] )
  fi
])

dnl
dnl $Id$
dnl
dnl Set target tools
dnl
dnl 98/06/23 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl		fixing cache/environment variable handling
dnl		adding checks for cygwin/egcs '\\'-bug
dnl		adding checks for ranlib/ar -s problem 
dnl
dnl 98/02/12 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl

AC_DEFUN(RTEMS_GCC_PRINT,
[ case $host_os in
  *cygwin*)
    dnl FIXME: Hack for cygwin/egcs reporting mixed '\\' and '/'
    dnl        Should be removed once cygwin/egcs reports '/' only
    $1=`$CC_FOR_TARGET --print-prog-name=$2 | sed -e 's%\\\\%/%g' `
    ;;
  *)
    $1=`$CC_FOR_TARGET --print-prog-name=$2`
    ;;
  esac
])

AC_DEFUN(RTEMS_PATH_TOOL,
[
AC_MSG_CHECKING([target's $2])
AC_CACHE_VAL(ac_cv_path_$1,:)
AC_MSG_RESULT([$ac_cv_path_$1])

if test -n "$ac_cv_path_$1"; then
  dnl retrieve the value from the cache
  $1=$ac_cv_path_$1
else
  dnl the cache was not set
  if test -z "[$]$1" ; then
    if test "$rtems_cv_prog_gcc" = "yes"; then
      # We are using gcc, ask it about its tool
      # NOTE: Necessary if gcc was configured to use the target's 
      # native tools or uses prefixes for gnutools (e.g. gas instead of as)
      RTEMS_GCC_PRINT($1,$2)
    fi
  else
    # The user set an environment variable.
    # Check whether it is an absolute path, otherwise AC_PATH_PROG 
    # will override the environment variable, which isn't what the user
    # intends
    AC_MSG_CHECKING([whether environment variable $1 is an absolute path])
    case "[$]$1" in
    /*) # valid
      AC_MSG_RESULT("yes")
    ;;
    *)  # invalid for AC_PATH_PROG
      AC_MSG_RESULT("no")
      AC_MSG_ERROR([***]
        [Environment variable $1 should either]
	[be unset (preferred) or contain an absolute path])
    ;;
    esac
  fi

  AC_PATH_PROG($1,"$program_prefix"$2,$3)
fi
])

AC_DEFUN(RTEMS_CANONICALIZE_TOOLS,
[AC_REQUIRE([RTEMS_PROG_CC])dnl

dnl FIXME: What shall be done if these tools are not available?
  RTEMS_PATH_TOOL(AR_FOR_TARGET,ar,no)
  RTEMS_PATH_TOOL(AS_FOR_TARGET,as,no)
  RTEMS_PATH_TOOL(LD_FOR_TARGET,ld,no)
  RTEMS_PATH_TOOL(NM_FOR_TARGET,nm,no)

dnl special treatment of ranlib
  RTEMS_PATH_TOOL(RANLIB_FOR_TARGET,ranlib,no)
  if test "$RANLIB_FOR_TARGET" = "no"; then
    # ranlib wasn't found; check if ar -s is available
    RTEMS_AR_FOR_TARGET_S
    if test $rtems_cv_AR_FOR_TARGET_S = "yes" ; then
      dnl override RANLIB_FOR_TARGET's cache
      ac_cv_path_RANLIB_FOR_TARGET="$AR_FOR_TARGET -s"
      RANLIB_FOR_TARGET=$ac_cv_path_RANLIB_FOR_TARGET
    else
      AC_MSG_ERROR([***]
        [Can't figure out how to build a library index]
	[Neither ranlib nor ar -s seem to be available] )
    fi
  fi

dnl NOTE: These may not be available if not using gnutools
  RTEMS_PATH_TOOL(OBJCOPY_FOR_TARGET,objcopy,no)
  RTEMS_PATH_TOOL(SIZE_FOR_TARGET,size,no)
  RTEMS_PATH_TOOL(STRIP_FOR_TARGET,strip,no)
])

dnl
dnl $Id$
dnl

AC_DEFUN(RTEMS_AR_FOR_TARGET_S,
[
AC_CACHE_CHECK(whether $AR_FOR_TARGET -s works,
rtems_cv_AR_FOR_TARGET_S,
[
cat > conftest.$ac_ext <<EOF
int foo( int b ) 
{ return b; }
EOF
if AC_TRY_COMMAND($CC_FOR_TARGET -o conftest.o -c conftest.$ac_ext) \
  && AC_TRY_COMMAND($AR_FOR_TARGET -sr conftest.a conftest.o) \
  && test -s conftest.a ; \
then
  rtems_cv_AR_FOR_TARGET_S="yes"
else
  rtems_cv_AR_FOR_TARGET_S="no"
fi
  rm -f conftest*
])
])


dnl
dnl  $Id$
dnl 

dnl check for i386 gas supporting 16 bit mode
dnl     - binutils 2.9.1.0.7 and higher

AC_DEFUN(RTEMS_I386_GAS_CODE16,
[ if test "${target_cpu}" = "i386"; then
    AC_CACHE_CHECK([for 16 bit mode assembler support],
      rtems_cv_prog_gas_code16,
      [cat > conftest.s << EOF
         .code16
         data32
         addr32
         lgdt 0
EOF
      if AC_TRY_COMMAND($AS_FOR_TARGET -o conftest.o conftest.s); then
        rtems_cv_prog_gas_code16=yes
      else
        rtems_cv_prog_gas_code16=no
      fi])
    RTEMS_GAS_CODE16="$rtems_cv_prog_gas_code16"
  fi
  AC_SUBST(RTEMS_GAS_CODE16)
])


dnl
dnl $Id$
dnl
dnl Check for System V IPC calls used by Unix simulators
dnl
dnl 98/07/17 Dario Alcocer     alcocer@netcom.com
dnl 	     Ralf Corsepius    corsepiu@faw.uni-ulm.de
dnl
dnl Note: $host_os should probably *not* ever be used here to
dnl determine if host supports System V IPC calls, since some
dnl (e.g. FreeBSD 2.x) are configured by default to include only
dnl a subset of the System V IPC calls.  Therefore, to make sure
dnl all of the required calls are found, test for each call explicitly.
dnl
dnl All of the calls use IPC_PRIVATE, so tests will not unintentionally
dnl modify any existing key sets.  See the man pages for semget, shmget, 
dnl msgget, semctl, shmctl and msgctl for details.

AC_DEFUN(RTEMS_SYSV_SEM,
[AC_REQUIRE([AC_PROG_CC]) 
AC_REQUIRE([RTEMS_CANONICAL_HOST])
AC_CACHE_CHECK(whether $RTEMS_HOST supports System V semaphores,
rtems_cv_sysv_sem,
[
AC_TRY_RUN([
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
int main () {
#if !defined(sun) 
  union semun arg ;
#else
  union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
  } arg;
#endif
  int id=semget(IPC_PRIVATE,1,IPC_CREAT|0400);
  if (id == -1)
    exit(1);
  arg.val = 0; /* avoid implicit type cast to union */
  if (semctl(id, 0, IPC_RMID, arg) == -1)
    exit(1);
  exit(0);
}
],
rtems_cv_sysv_sem="yes", rtems_cv_sysv_sem="no", :)
])
])

AC_DEFUN(RTEMS_SYSV_SHM,
[AC_REQUIRE([AC_PROG_CC]) 
AC_REQUIRE([RTEMS_CANONICAL_HOST])
AC_CACHE_CHECK(whether $RTEMS_HOST supports System V shared memory,
rtems_cv_sysv_shm,
[
AC_TRY_RUN([
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
int main () {
  int id=shmget(IPC_PRIVATE,1,IPC_CREAT|0400);
  if (id == -1)
    exit(1);
  if (shmctl(id, IPC_RMID, 0) == -1)
    exit(1);
  exit(0);
}
],
rtems_cv_sysv_shm="yes", rtems_cv_sysv_shm="no", :)
])
])

AC_DEFUN(RTEMS_SYSV_MSG,
[AC_REQUIRE([AC_PROG_CC]) 
AC_REQUIRE([RTEMS_CANONICAL_HOST])
AC_CACHE_CHECK(whether $RTEMS_HOST supports System V messages,
rtems_cv_sysv_msg,
[
AC_TRY_RUN([
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int main () {
  int id=msgget(IPC_PRIVATE,IPC_CREAT|0400);
  if (id == -1)
    exit(1);
  if (msgctl(id, IPC_RMID, 0) == -1)
    exit(1);
  exit(0);
}
],
rtems_cv_sysv_msg="yes", rtems_cv_sysv_msg="no", :)
])
])

AC_DEFUN(RTEMS_CHECK_SYSV_UNIX,
[AC_REQUIRE([RTEMS_CANONICAL_HOST])
if test "$RTEMS_CPU" = "unix" ; then
  RTEMS_SYSV_SEM
  if test "$rtems_cv_sysv_sem" != "yes" ; then
    AC_MSG_ERROR([System V semaphores don't work, required by simulator])
  fi
  RTEMS_SYSV_SHM
  if test "$rtems_cv_sysv_shm" != "yes" ; then
    AC_MSG_ERROR([System V shared memory doesn't work, required by simulator])
  fi
  RTEMS_SYSV_MSG
  if test "$rtems_cv_sysv_msg" != "yes" ; then
    AC_MSG_ERROR([System V messages don't work, required by simulator])
  fi
fi
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
case "${target}" in
changequote(,)dnl
  i[3456]86-go32-rtems*)
changequote([,])dnl
    $1="go32 go32_p5"
    ;;
  *)
    files=`ls $srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU`
    for file in $files; do
      case $file in
        shared*);;
        Makefile*);;
        READ*);;
        CVS*);;
        pxfl*);;
        go32*);;       # so the i386 port can pick up the other Makefiles
        # Now account for BSPs with build variants
        gen68360)      rtems_bsp="$rtems_bsp gen68360 gen68360_040";;
        p4000)         rtems_bsp="$rtems_bsp p4600 p4650";;
        mvme162)       rtems_bsp="$rtems_bsp mvme162 mvme162lx";;
        *) $1="[$]$1 $file";;
      esac;
    done
    ;;
esac
AC_MSG_RESULT([[$]$1 .. done])
])dnl

AC_DEFUN(RTEMS_CHECK_CUSTOM_BSP,
[dnl
AC_MSG_CHECKING([for make/custom/[$]$1.cfg])
if test -r "$srcdir/$RTEMS_TOPdir/make/custom/[$]$1.cfg"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_ERROR([no])
fi
])dnl

dnl
dnl $Id$
dnl

dnl RTEMS_CHECK_MAKEFILE(path)
dnl Search for Makefile.in's within the directory starting
dnl at path and append an entry for Makefile to global variable 
dnl "makefiles" (from configure.in) for each Makefile.in found
dnl 
AC_DEFUN(RTEMS_CHECK_MAKEFILE,
[RTEMS_CHECK_FILES_IN($1,Makefile,makefiles)
])

dnl
dnl $Id$
dnl

dnl RTEMS_CHECK_FILES_IN(path,file,var)
dnl path .. path relative to srcdir, where to start searching for files
dnl file .. name of the files to search for
dnl var  .. shell variable to append files found

AC_DEFUN(RTEMS_CHECK_FILES_IN,
[
AC_MSG_CHECKING(for $2.in in $1)
if test -d $srcdir/$1; then
  rtems_av_save_dir=`pwd`;
  cd $srcdir;
  rtems_av_tmp=`find $1 -name "$2.in" -print | sed "s/$2\.in/%/" | sort | sed "s/%/$2/"`
  $3="$$3 $rtems_av_tmp";
  cd $rtems_av_save_dir;
  AC_MSG_RESULT(done)
else
  AC_MSG_RESULT(no)
fi
])


