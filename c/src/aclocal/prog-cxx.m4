dnl
dnl Check for target g++
dnl

AC_DEFUN([RTEMS_PROG_CXX_FOR_TARGET],
[
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl
AC_REQUIRE([RTEMS_ENABLE_CXX])
AC_REQUIRE([RTEMS_CHECK_SMP])

# If CXXFLAGS is not set, default to CFLAGS
if test x"$rtems_cv_HAS_SMP" = x"yes" ; then
  CXXFLAGS=${CXXFLAGS-${CFLAGS} -std=gnu++11}
else
  CXXFLAGS=${CXXFLAGS-${CFLAGS}}
fi

CXXFLAGS=`echo ${CXXFLAGS} | sed \
  -e s/-Wmissing-prototypes// \
  -e s/-Wimplicit-function-declaration// \
  -e s/-Wstrict-prototypes// \
  -e s/-Wnested-externs//`

RTEMS_CHECK_TOOL(CXX,g++)
if test "$RTEMS_HAS_CPLUSPLUS" = "yes";
then
  dnl Only accept g++
  dnl NOTE: This might be too restrictive
  if test -z "$CXX";
  then
    RTEMS_HAS_CPLUSPLUS=no
    HAS_CPLUSPLUS=no
    ## Work-around to a bug in automake
    AM_CONDITIONAL([am__fastdepCXX],[false])
  else
    AC_PROG_CXX
    AC_PROG_CXXCPP
    if test "$ac_cv_prog_cc_cross" != "$ac_cv_prog_cxx_cross"; then
      AC_MSG_ERROR([***]
       [Inconsistency in compiler configuration:]
       [Target C compiler and target C++ compiler]
       [must both either be cross compilers or native compilers])
    fi
  fi
else
  ## Work-around to a bug in automake
  AM_CONDITIONAL([am__fastdepCXX],[false])
fi
])
