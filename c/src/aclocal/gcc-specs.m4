dnl Check whether the target compiler accepts -specs

AC_DEFUN([RTEMS_GCC_SPECS],
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
