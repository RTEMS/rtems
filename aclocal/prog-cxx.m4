dnl
dnl $Id$
dnl 
dnl Check for target g++
dnl 
dnl 98/05/20 Ralf Corsepius 	(corsepiu@faw.uni-ulm.de)
dnl				Completely reworked

AC_DEFUN(RTEMS_PROG_CXX,
[
AC_BEFORE([$0], [AC_PROG_CXXCPP])dnl
AC_BEFORE([$0], [AC_PROG_CXX])dnl
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl
AC_REQUIRE([RTEMS_TOOL_PREFIX])dnl

dnl Only accept g++ and c++
dnl NOTE: This might be too restrictive for native compilation
AC_PATH_PROGS(CXX_FOR_TARGET, "$program_prefix"g++ "$program_prefix"c++)
test -z "$CXX_FOR_TARGET" \
  && AC_MSG_ERROR([no acceptable c++ found in \$PATH])

dnl backup 
rtems_save_CXX=$CXX
rtems_save_CXXFLAGS=$CXXFLAGS

dnl temporarily set CXX
CXX=$CXX_FOR_TARGET

AC_PROG_CXX_WORKS
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

rtems_cv_prog_gxx=$ac_cv_prog_gxx
rtems_cv_prog_cxx_g=$ac_cv_prog_cxx_g
rtems_cv_prog_cxx_works=$ac_cv_prog_cxx_works
rtems_cv_prog_cxx_cross=$ac_cv_prog_cxx_cross

CXX=$rtems_save_CXX
CXXFLAGS=$rtems_save_CXXFLAGS

dnl restore initial values
unset ac_cv_prog_gxx
unset ac_cv_prog_cc_g
unset ac_cv_prog_cxx_works
unset ac_cv_prog_cxx_cross
])

AC_DEFUN(RTEMS_PROG_CXX_FOR_TARGET,
[
  RTEMS_PROG_CXX
  if test "$rtems_cv_prog_cc_cross" != "$rtems_cv_prog_cxx_cross"; then
    AC_MSG_ERROR([***]
     [Inconsistency in compiler configuration:]
     [Target C compiler and Target C++ compiler]
     [must both either be cross compilers or native compilers]
     [Hint: If building a posix bsp: LD_LIBRARY_PATH?] )
  fi
])
