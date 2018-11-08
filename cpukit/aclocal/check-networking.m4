dnl
AC_DEFUN([RTEMS_CHECK_NETWORKING],
[dnl
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_NETWORKING])dnl
AC_REQUIRE([RTEMS_INCLUDES])dnl

AC_CACHE_CHECK([whether CPU supports networking],
  rtems_cv_HAS_NETWORKING,
  [dnl
    case "$host" in
    # do not have address space to hold BSD TCP/IP stack
    epiphany*)
      rtems_cv_HAS_NETWORKING="no"
      ;;
    # Newer architecture ports that should only use new TCP/IP stack
    x86_64*)
      rtems_cv_HAS_NETWORKING="no"
      ;;
    *)
      AS_IF([test "${RTEMS_HAS_NETWORKING}" = "yes"],
      [rtems_cv_HAS_NETWORKING="yes"
      RTEMS_CPPFLAGS="${RTEMS_CPPFLAGS} -I${RTEMS_SOURCE_ROOT}/cpukit/libnetworking"],
      [rtems_cv_HAS_NETWORKING="no"])
      ;;
    esac
    ])
  ])
])
