AC_DEFUN(RTEMS_CHECK_RDBG,
[dnl
AC_REQUIRE([RTEMS_TOP])dnl
AC_REQUIRE([RTEMS_CHECK_CPU])dnl
AC_CACHE_CHECK([whether cpu supports librdbg],
  rtems_cv_has_rdbg,
  [
    if test -d "$srcdir/${RTEMS_TOPdir}/c/src/lib/librdbg/${RTEMS_CPU}"; then
      rtems_cv_has_rdbg="yes" ;
    else
      rtems_cv_has_rdbg="no";
    fi
  ])
])
