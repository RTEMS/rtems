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
