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
test "$rtems_cv_gcc_pipe" = "yes" && CC="$CC --pipe"

if test "$GCC" = yes; then
]
m4_if([$1],,[],[CPPFLAGS="$CPPFLAGS $1"])
[
CFLAGS="-g -Wall"
fi

dnl FIXME: HACK for egcs/cygwin mixing '\\' and '/' in gcc -print-*
#case $build_os in
#*cygwin*)     GCCSED="| sed 's%\\\\%/%g'" ;;
#*) ;;
#esac
AC_SUBST(GCCSED)
])
