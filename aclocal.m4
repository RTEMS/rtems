dnl aclocal.m4 generated automatically by aclocal 1.2

dnl $Id$

dnl macro to detect mkdir
AC_DEFUN(RTEMS_PATH_MKDIR,
[AC_PATH_PROG(MKDIR,mkdir)
])

dnl macro to check for mkdir -p
AC_DEFUN(RTEMS_PROG_MKDIR_P,
[
AC_REQUIRE([RTEMS_PATH_MKDIR])
AC_MSG_CHECKING([for working $MKDIR -p])
AC_CACHE_VAL(rtems_cv_prog_mkdir_p,
[rm -rf conftestdata
if $MKDIR -p conftestdata 2>/dev/null ;then
rtems_cv_prog_MKDIR_P="yes"
else
rtems_cv_prog_MKDIR_P="no"
fi])dnl
rm -rf conftestdata
AC_MSG_RESULT($rtems_cv_prog_MKDIR_P)
])

dnl macro to check for mkdir -m 0755
AC_DEFUN(RTEMS_PROG_MKDIR_M,
[
AC_REQUIRE([RTEMS_PATH_MKDIR])
AC_MSG_CHECKING([for working $MKDIR -m 0755])
AC_CACHE_VAL(rtems_cv_prog_MKDIR_P,
[rm -rf conftestdata
if $MKDIR -m 0775 conftestdata 2>/dev/null; then
rtems_cv_prog_MKDIR_M="yes"
else
rtems_cv_prog_MKDIR_M="no"
fi])dnl
rm -rf conftestdata
AC_MSG_RESULT($rtems_cv_prog_MKDIR_M)
])


dnl $Id$

dnl canonicalize target name
dnl NOTE: Most rtems targets do not fullfil autoconf's
dnl target naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN(RTEMS_CANONICAL_TARGET_CPU,
[AC_MSG_CHECKING(rtems target cpu)
changequote(<<, >>)dnl
target_cpu=`echo $target | sed 's%^\([^-]*\)-\(.*\)$%\1%'`
changequote([, ])dnl
AC_MSG_RESULT($target_cpu)
])

dnl
dnl $Id$
dnl 
dnl Check for target gcc
dnl
dnl Adaptation of autoconf-2.12's AC_PROG_CC to rtems
dnl
dnl 98/02/10 Ralf Corsepius 	(corsepiu@faw.uni-ulm.de)
dnl

AC_DEFUN(RTEMS_PROG_CC,
[
AC_BEFORE([$0], [AC_PROG_CPP])dnl
AC_BEFORE([$0], [AC_PROG_CC])dnl
AC_CHECK_PROG(CC, gcc, gcc)
if test -z "$CC"; then
  AC_CHECK_PROG(CC, cc, cc, , , /usr/ucb/cc)
  test -z "$CC" && AC_MSG_ERROR([no acceptable cc found in \$PATH])
fi

RTEMS_PROG_CC_WORKS
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

CC_FOR_TARGET=$CC
rtems_cv_prog_gcc=$ac_cv_prog_gcc
rtems_cv_prog_cc_g=$ac_cv_prog_cc_g

dnl restore initial values
unset CC
unset ac_cv_prog_gcc
unset ac_cv_prog_cc_g
unset ac_cv_prog_CC
])


dnl Almost identical to AC_PROG_CC_WORKS
dnl added malloc to program fragment, because rtems has its own malloc
dnl which is not available while bootstrapping rtems

AC_DEFUN(RTEMS_PROG_CC_WORKS,
[AC_MSG_CHECKING([whether the target C compiler ($CC $CFLAGS $LDFLAGS) works])
AC_LANG_SAVE
AC_LANG_C
AC_TRY_COMPILER(
[
#if defined(__PPC__)
  int __SDATA_START__;  int __SDATA2_START__;
  int __GOT_START__;    int __GOT_END__;
  int __GOT2_START__;   int __GOT2_END__;
  int __SBSS_END__;     int __SBSS2_END__;
  int __FIXUP_START__;  int __FIXUP_END__;
  int __EXCEPT_START__; int __EXCEPT_END__;
  int __init;           int __fini;
#endif
#if defined(__hppa__)
  asm ( ".subspa \$GLOBAL\$,QUAD=1,ALIGN=8,ACCESS=0x1f,SORT=40");
  asm ( ".export \$global\$" );
  asm ( "\$global\$:");
#endif
 int atexit() { return 0; }
 void *malloc() { return 0; } 
 main(){return(0);}], 
rtems_cv_prog_cc_works, rtems_cv_prog_cc_cross)
AC_LANG_RESTORE
AC_MSG_RESULT($rtems_cv_prog_cc_works)
if test $rtems_cv_prog_cc_works = no; then
  AC_MSG_ERROR([installation or configuration problem: target C compiler cannot create executables.])
fi
AC_MSG_CHECKING([whether the target C compiler ($CC $CFLAGS $LDFLAGS) is a cross-compiler])
AC_MSG_RESULT($rtems_cv_prog_cc_cross)
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
[touch confspec
echo 'void f(){}' >conftest.c
if test -z "`${CC_FOR_TARGET} -specs confspec -c conftest.c 2>&1`";then
  rtems_cv_gcc_specs=yes
else
  rtems_cv_gcc_specs=no
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
case "$host_os" in
  cygwin32*)
    rtems_cv_gcc_pipe=no 
    ;;
  *)
    echo 'void f(){}' >conftest.c
    if test -z "`${CC_FOR_TARGET} --pipe -c conftest.c 2>&1`";then
      rtems_cv_gcc_pipe=yes
    else
      rtems_cv_gcc_pipe=no
    fi
    rm -f conftest*
    ;;
esac
])
])

dnl
dnl $Id$
dnl 
dnl Check for target g++
dnl 
dnl Adaptation of autoconf-2.12's AC_PROG_CXX to rtems
dnl
dnl 98/02/10 Ralf Corsepius (corsepiu@faw.uni-ulm.de)
dnl
 
AC_DEFUN(RTEMS_PROG_CXX,
[
AC_BEFORE([$0], [AC_PROG_CXXCPP])dnl
AC_BEFORE([$0], [AC_PROG_CXX])dnl
AC_CHECK_PROGS(CXX, $CCC c++ g++ gcc CC cxx cc++, gcc)

RTEMS_PROG_CXX_WORKS
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
CXX_FOR_TARGET=$CXX

dnl restore initial values
unset CXX
unset ac_cv_prog_gxx
])


dnl Almost identical to AC_PROG_CXX_WORKS
dnl Additional handling of malloc
dnl NOTE: using newlib with a native compiler is cross-compiling, indeed.
AC_DEFUN(RTEMS_PROG_CXX_WORKS,
[AC_MSG_CHECKING([whether the target C++ compiler ($CXX $CXXFLAGS $LDFLAGS) works])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS

dnl this fails if rtems uses newlib, because rtems has its own malloc which
dnl is not available at bootstrap
AC_TRY_COMPILER(
  [main(){return(0);}], 
  rtems_cv_prog_cxx_works, rtems_cv_prog_cxx_cross)
if test "$rtems_cv_prog_cxx_works" = "no"; then
  dnl now retry with our own version of malloc
  AC_TRY_COMPILER(
    [extern "C" void *malloc(); void *malloc() { return (0); }
     extern "C" int atexit();   int atexit() { return 0; }
#if defined(__PPC__)
  int __SDATA_START__;  int __SDATA2_START__;
  int __GOT_START__;    int __GOT_END__;
  int __GOT2_START__;   int __GOT2_END__;
  int __SBSS_END__;     int __SBSS2_END__;
  int __FIXUP_START__;  int __FIXUP_END__;
  int __EXCEPT_START__; int __EXCEPT_END__;
  int __init;           int __fini;
#endif
#if defined(__hppa__)
  asm ( ".subspa \$GLOBAL\$,QUAD=1,ALIGN=8,ACCESS=0x1f,SORT=40");
  asm ( ".export \$global\$" );
  asm ( "\$global\$:");
#endif

     main(){return(0);}],
    rtems_cv_prog_cxx_works, rtems_cv_prog_cxx_cross)
fi
AC_LANG_RESTORE
AC_MSG_RESULT($rtems_cv_prog_cxx_works)
if test $rtems_cv_prog_cxx_works = no; then
  AC_MSG_ERROR([installation or configuration problem: target C++ compiler cannot create executables.])
fi
AC_MSG_CHECKING([whether the target C++ compiler ($CXX $CXXFLAGS $LDFLAGS) is a cross-compiler])
AC_MSG_RESULT($rtems_cv_prog_cxx_cross)
])

dnl $Id$
dnl
dnl Set target tools
dnl
dnl 98/02/12 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl

AC_DEFUN(RTEMS_CANONICALIZE_TOOLS,
[AC_REQUIRE([RTEMS_PROG_CC])dnl
if test "$rtems_cv_prog_gcc" = "yes" ; then
  dnl We are using gcc, now ask it about its tools
  dnl Necessary if gcc was configured to use the target's native tools
  dnl or uses prefixes for gnutools (e.g. gas instead of as)
  AR_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=ar`
  AS_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=as`
  LD_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=ld`
  NM_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=nm`
  RANLIB_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=ranlib`
fi

dnl check whether the tools exist
dnl FIXME: What shall be done if they don't exist?

dnl NOTE: CC_FOR_TARGET should always be valid at this point, 
dnl       cf. RTEMS_PROG_CC  
AC_PATH_PROG(CC_FOR_TARGET,"$program_prefix"gcc,no)

dnl FIXME: This may fail if the compiler has not been recognized as gcc
dnl       and uses tools with different names
AC_PATH_PROG(AR_FOR_TARGET,"$program_prefix"ar,no)
AC_PATH_PROG(AS_FOR_TARGET,"$program_prefix"as,no)
AC_PATH_PROG(NM_FOR_TARGET,"$program_prefix"nm,no)
AC_PATH_PROG(LD_FOR_TARGET,"$program_prefix"ld,no)

dnl NOTE: This is doubtful, but should not disturb all current rtems'
dnl 	  targets (remark: solaris fakes ranlib!!)
AC_PATH_PROG(RANLIB_FOR_TARGET,"$program_prefix"ranlib,no)

dnl NOTE: These may not be available, if not using gnutools
AC_PATH_PROG(OBJCOPY_FOR_TARGET,"$program_prefix"objcopy,no)
AC_PATH_PROG(SIZE_FOR_TARGET,"$program_prefix"size,no)
])

dnl $Id$

dnl RTEMS_CHECK_MAKEFILE(path)
dnl Search for Makefile.in's within the directory starting
dnl at path and append an entry for Makefile to global variable 
dnl "makefiles" (from configure.in) for each Makefile.in found
dnl 
AC_DEFUN(RTEMS_CHECK_MAKEFILE,
[RTEMS_CHECK_FILES_IN($1,Makefile,makefiles)
])

dnl $Id$

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


