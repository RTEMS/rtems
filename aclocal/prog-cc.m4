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
