# Check whether FUNCTION is declared in INCLUDES
# and whether rtems_stub_FUNCTION or FUNCTION is supplied.

# RTEMS_CHECK_FUNC(FUNCTION[,INCLUDES])
AC_DEFUN([RTEMS_CHECK_FUNC],
[AC_REQUIRE([RTEMS_CHECK_NEWLIB])
AC_CHECK_DECLS([$1],,,[$2])

AC_CACHE_CHECK([for $1],[ac_cv_$1],[
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([[$2]],[[rtems_stub_$1()]])],
    [ac_cv_$1="stub"],
    [AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[$2]],[[$1()]])],
      [ac_cv_$1="yes"],
      [ac_cv_$1="no"])
    ])
  ])
AS_IF([test "$ac_cv_$1" = yes],
  [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_]$1),[1],[Define to 1 if you have the `$1' function.])])
])
