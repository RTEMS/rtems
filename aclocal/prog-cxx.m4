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
