AC_DEFUN([_RTEMS_CHECK_CUSTOM_BSP],[
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_TOP])dnl sets RTEMS_TOPdir
  for i in ${rtems_rootdir}make/custom/$1;
  do
    AC_MSG_CHECKING([for $i])
    AS_IF([test -r $i],[
      $2="$i"
      AC_MSG_RESULT([yes])
      break;
    ],[
      AC_MSG_RESULT([no])
    ])
  done
])

AC_DEFUN([RTEMS_CHECK_CUSTOM_BSP],[
  _RTEMS_CHECK_CUSTOM_BSP([[$]$1.cfg],[BSP_FOUND])
  AS_IF([test -z "$BSP_FOUND"],[
    AC_MSG_ERROR([missing [$]$1.cfg])
  ])
])
