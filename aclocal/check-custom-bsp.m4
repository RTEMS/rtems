AC_DEFUN([_RTEMS_CHECK_CUSTOM_BSP],[
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_SOURCE_TOP])dnl sets RTEMS_SOURCE_ROOT
  $2=
  for i in \
    `ls "${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}"/*/config/$1 2>/dev/null`;
  do
    AS_IF([test -r $i],[
      $2="$i"
      break;
    ])
  done
])

AC_DEFUN([RTEMS_CHECK_CUSTOM_BSP],[
  _RTEMS_CHECK_CUSTOM_BSP([[$]$1.cfg],[BSP_FOUND])
  AS_IF([test -z "$BSP_FOUND"],[
    AC_MSG_ERROR([missing [$]$1.cfg])
  ])
])
