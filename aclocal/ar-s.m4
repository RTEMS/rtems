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

