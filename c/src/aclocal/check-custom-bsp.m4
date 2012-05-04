AC_DEFUN([_RTEMS_CHECK_CUSTOM_BSP],[
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_TOP])dnl sets RTEMS_TOPdir
  $2=
  for i in \
    `ls "${srcdir}/${RTEMS_TOPdir}/c/src/lib/libbsp/${RTEMS_CPU}"/*/make/custom/$1 2>/dev/null`;
  do
    AS_IF([test -r $i],[
      $2="$i"
      break
    ])
  done
])

AC_DEFUN([RTEMS_CHECK_CUSTOM_BSP],[
  _RTEMS_CHECK_CUSTOM_BSP([[$]$1.cfg],[BSP_FOUND])
  AS_IF([test -z "$BSP_FOUND"],[
    AC_MSG_ERROR([missing [$]$1.cfg])
  ])
])
