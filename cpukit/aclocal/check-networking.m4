dnl
AC_DEFUN([RTEMS_CHECK_NETWORKING],
[dnl
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_NETWORKING])dnl

AC_CACHE_CHECK([whether CPU supports networking],
  rtems_cv_HAS_NETWORKING,
  [dnl
    case "$host" in
    *-*-rtems*)
      AS_IF([test "${RTEMS_HAS_NETWORKING}" = "yes"],[
# suppress libnetworking if one these types is not available
        AS_IF([test x"$ac_cv_type_int8_t" = xyes \
          && test x"$ac_cv_type_uint8_t" = xyes \
          && test x"$ac_cv_type_int16_t" = xyes \
          && test x"$ac_cv_type_uint16_t" = xyes \
          && test x"$ac_cv_type_int32_t" = xyes \
          && test x"$ac_cv_type_uint32_t" = xyes \
          && test x"$ac_cv_type_int64_t" = xyes \
          && test x"$ac_cv_type_uint64_t" = xyes],
        [rtems_cv_HAS_NETWORKING=yes],
        [rtems_cv_HAS_NETWORKING=no])
      ],[
        rtems_cv_HAS_NETWORKING=disabled
      ])
      ;;
    esac
    ])
  ])
])
