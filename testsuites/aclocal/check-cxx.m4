dnl
AC_DEFUN([RTEMS_CHECK_CXX],
[dnl
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl
AC_REQUIRE([RTEMS_PROG_CC_FOR_TARGET])dnl
AC_REQUIRE([RTEMS_PROG_CXX_FOR_TARGET])dnl
AC_CACHE_CHECK([whether to build rtems++],
  rtems_cv_HAS_CPLUSPLUS,
  [ if test "$RTEMS_HAS_CPLUSPLUS" = "yes"; then
      if test -n "$CXX"; then
        rtems_cv_HAS_CPLUSPLUS="yes"
      else
        rtems_cv_HAS_CPLUSPLUS="no"
      fi
    else
      rtems_cv_HAS_CPLUSPLUS="no"
    fi])
HAS_CPLUSPLUS="$rtems_cv_HAS_CPLUSPLUS";
AC_SUBST(HAS_CPLUSPLUS)dnl
])
