dnl
dnl $Id$
dnl
dnl Check whether the target compiler accepts -pipe
dnl

AC_DEFUN(RTEMS_GCC_PIPE,
[AC_REQUIRE([RTEMS_PROG_CC]) 
AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK(whether $CC_FOR_TARGET accepts --pipe,rtems_cv_gcc_pipe,
[
rtems_cv_gcc_pipe=no
if test "$rtems_cv_prog_gcc" = "yes"; then
dnl case "$host_os" in
dnl   cygwin*)
dnl    ;;
dnl  *)
    echo 'void f(){}' >conftest.c
    if test -z "`${CC_FOR_TARGET} --pipe -c conftest.c 2>&1`";then
      rtems_cv_gcc_pipe=yes
    fi
    rm -f conftest*
dnl    ;;
dnl esac
fi
])
])
