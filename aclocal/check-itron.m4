dnl $Id$
dnl
AC_DEFUN(RTEMS_CHECK_ITRON_API,
[dnl
AC_REQUIRE([RTEMS_CHECK_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_ITRON])dnl

AC_CACHE_CHECK([whether BSP supports libitron],
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
