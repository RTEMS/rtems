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
m4_if([$1],,[],[CPPFLAGS="$CPPFLAGS $1"])
[
CFLAGS="-g -Wall"
fi
## Conditional for automake files
AM_CONDITIONAL(RTEMS_USE_GCC272, test x"$RTEMS_USE_GCC272" = x"yes")
## Make variable for autoconf fragments (*.cfg)
AC_SUBST(RTEMS_USE_GCC272)

dnl FIXME: HACK for egcs/cygwin mixing '\\' and '/' in gcc -print-*
case $build_os in
*cygwin*)     GCCSED="| sed 's%\\\\%/%g'" ;;
*) ;;
esac
AC_SUBST(GCCSED)
])
