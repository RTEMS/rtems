dnl $Id$
dnl
AC_DEFUN([RTEMS_CHECK_ITRON_API],
[dnl
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_ITRON])dnl

AC_CACHE_CHECK([whether CPU supports libitron],
  rtems_cv_HAS_ITRON_API,
  [dnl
    case "$RTEMS_CPU" in
    unix*)
      rtems_cv_HAS_ITRON_API="no"
      ;;
    *)
      if test "${RTEMS_HAS_ITRON_API}" = "yes"; then
        rtems_cv_HAS_ITRON_API="yes";
      else
        rtems_cv_HAS_ITRON_API="disabled";
      fi
      ;;
    esac])
if test "$rtems_cv_HAS_ITRON_API" = "yes"; then
  HAS_ITRON_API="yes";
else
  HAS_ITRON_API="no";
fi
AC_SUBST(HAS_ITRON_API)dnl
])

AC_DEFUN([RTEMS_DEFINE_ITRON_API],
[AC_REQUIRE([RTEMS_CHECK_ITRON_API])dnl
if test x"${HAS_ITRON_API}" = x"yes";
then
  AC_DEFINE_UNQUOTED(RTEMS_ITRON_API,1,[if itron api is supported])
fi
])
