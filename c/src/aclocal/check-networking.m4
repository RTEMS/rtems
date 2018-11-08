AC_DEFUN([RTEMS_CHECK_NETWORKING],
[dnl
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_NETWORKING])dnl
AC_REQUIRE([RTEMS_INCLUDES])dnl

AC_CACHE_CHECK([whether BSP supports networking],
  rtems_cv_HAS_NETWORKING,
  [dnl
    case "$RTEMS_CPU" in
    # do not have address space to hold BSD TCP/IP stack
    epiphany*)
      rtems_cv_HAS_NETWORKING="no"
      ;;
    # Newer architecture ports that should only use new TCP/IP stack
    x86_64*)
      rtems_cv_HAS_NETWORKING="no"
      ;;
    *)
      if test "${RTEMS_HAS_NETWORKING}" = "yes"; then
        rtems_cv_HAS_NETWORKING="yes";
      else
        rtems_cv_HAS_NETWORKING="disabled";
      fi
      ;;
    esac])
if test "$rtems_cv_HAS_NETWORKING" = "yes"; then
  HAS_NETWORKING="yes";
  RTEMS_CPPFLAGS="${RTEMS_CPPFLAGS} -I${RTEMS_SOURCE_ROOT}/cpukit/libnetworking";
else
  HAS_NETWORKING="no";
fi
AC_SUBST(HAS_NETWORKING)dnl
])
