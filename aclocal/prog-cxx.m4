dnl
dnl $Id$
dnl 
dnl Check for target g++
dnl 

AC_DEFUN([RTEMS_PROG_CXX_FOR_TARGET],
[
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl
AC_REQUIRE([RTEMS_ENABLE_CXX])
RTEMS_CHECK_TOOL(CXX,g++)
if test "$RTEMS_HAS_CPLUSPLUS" = "yes";
then
_RTEMS_FLAGS([CXXFLAGS],
  ["\$(CPU_CFLAGS) \$(RTEMS_CFLAGS_\$(VARIANT_V)_V) \$(CFLAGS_\$(VARIANT_V)_V) -g"])

dnl Only accept g++
dnl NOTE: This might be too restrictive
test -z "$CXX" \
  && AC_MSG_ERROR([no acceptable c++ found in \$PATH])
AC_PROG_CXX

  if test "$ac_cv_prog_cc_cross" != "$ac_cv_prog_cxx_cross"; then
    AC_MSG_ERROR([***]
     [Inconsistency in compiler configuration:]
     [Target C compiler and target C++ compiler]
     [must both either be cross compilers or native compilers]
     [Hint: If building a posix bsp: LD_LIBRARY_PATH?] )
  fi

else
## Work-around to a bug in automake
AM_CONDITIONAL([am__fastdepCXX],[false])
fi
])
