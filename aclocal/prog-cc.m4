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
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl
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
