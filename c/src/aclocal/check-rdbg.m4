dnl $Id$
dnl
AC_DEFUN([RTEMS_CHECK_RDBG],
[dnl
AC_REQUIRE([RTEMS_TOP])dnl
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl
AC_REQUIRE([RTEMS_CHECK_NETWORKING])dnl
AC_REQUIRE([RTEMS_ENABLE_RDBG])dnl
AC_CACHE_CHECK([whether BSP supports librdbg],
  rtems_cv_HAS_RDBG,
  [
    if test -d "$srcdir/${RTEMS_TOPdir}/c/src/librdbg/src/${RTEMS_CPU}/${$1}";
    then
      rtems_cv_HAS_RDBG="yes" ;
    elif test -d "$srcdir/${RTEMS_TOPdir}/c/src/librdbg/src/${RTEMS_CPU}/any"; 
    then
      rtems_cv_HAS_RDBG="yes" ;
    elif test "${RTEMS_CPU}" = "powerpc"; 
    then
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM(
          [],
          [#if defined(_OLD_EXCEPTIONS)
            choke me
           #endif])],
          [rtems_cv_HAS_RDBG=yes],
          [rtems_cv_HAS_RDBG=no])
    else
      rtems_cv_HAS_RDBG="no";
    fi
  ])
HAS_RDBG="$rtems_cv_HAS_RDBG"
])
