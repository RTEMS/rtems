dnl
dnl $Id$
dnl 
dnl Check for target g++
dnl 

AC_DEFUN(RTEMS_PROG_CXX,
[
AC_BEFORE([$0], [AC_PROG_CXXCPP])dnl
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl

_RTEMS_FLAGS([CXXFLAGS],
  ["\$(CPU_CFLAGS) \$(RTEMS_CFLAGS_\$(VARIANT_V)_V) \$(CFLAGS_\$(VARIANT_V)_V) -g"])

dnl Only accept g++
dnl NOTE: This might be too restrictive
RTEMS_CHECK_TOOL(CXX,g++)
test -z "$CXX" \
  && AC_MSG_ERROR([no acceptable c++ found in \$PATH])
AC_PROG_CXX

#if test "$GCC" = yes; then
#RTEMS_CXXFLAGS="$RTEMS_CXXFLAGS -Wall"
#m4_if([$1],,[],[RTEMS_CXXFLAGS="$RTEMS_CXXFLAGS $1"])
#fi
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
