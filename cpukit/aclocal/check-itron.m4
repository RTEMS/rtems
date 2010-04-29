dnl $Id$
dnl
AC_DEFUN([RTEMS_CHECK_ITRON_API],
[dnl
AC_REQUIRE([RTEMS_ENABLE_ITRON])dnl

AC_CACHE_CHECK([whether CPU supports libitron],
  rtems_cv_HAS_ITRON_API,
  [dnl
    AS_IF([test "${enable_itron}" = "yes"],[
# suppress itron if one these types is not available
      AS_IF([test x"$ac_cv_type_int8_t" = xyes \
        && test x"$ac_cv_type_uint8_t" = xyes \
        && test x"$ac_cv_type_int16_t" = xyes \
        && test x"$ac_cv_type_uint16_t" = xyes],
      [rtems_cv_HAS_ITRON_API=yes],
      [rtems_cv_HAS_ITRON_API=no])
    ],[
      rtems_cv_HAS_ITRON_API="disabled"
    ])
  ])
])
