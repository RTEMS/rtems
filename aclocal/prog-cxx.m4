dnl
dnl $Id$
dnl 
dnl Check for target g++
dnl 

AC_DEFUN(RTEMS_PROG_CXX,
[
AC_BEFORE([$0], [AC_PROG_CXXCPP])dnl
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl

dnl Only accept g++
dnl NOTE: This might be too restrictive
RTEMS_CHECK_TOOL(CXX,g++)
AC_PROG_CXX
test -z "$CXX" \
  && AC_MSG_ERROR([no acceptable c++ found in \$PATH])
])

AC_DEFUN(RTEMS_PROG_CXX_FOR_TARGET,
[
  RTEMS_PROG_CXX
  if test "$ac_cv_prog_cc_cross" != "$ac_cv_prog_cxx_cross"; then
    AC_MSG_ERROR([***]
     [Inconsistency in compiler configuration:]
     [Target C compiler and target C++ compiler]
     [must both either be cross compilers or native compilers]
     [Hint: If building a posix bsp: LD_LIBRARY_PATH?] )
  fi
])
