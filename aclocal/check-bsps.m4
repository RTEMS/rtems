dnl Report all available bsps for a target within the source tree
dnl
dnl RTEMS_CHECK_BSPS(bsp_list)
AC_DEFUN([RTEMS_CHECK_BSPS],
[
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_SOURCE_TOP])dnl sets RTEMS_SOURCE_ROOT

AC_MSG_CHECKING([for available BSPs])
  $1=
  for bsp_make in `echo "${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}"/*/config 2>/dev/null`; do
    bsp_family=`echo "$bsp_make" | sed \
      -e "s,^${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}/,," \
      -e "s,/config$,,"`
    for bsp_cfgs in `ls "${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}/$bsp_family/config/"*.cfg 2>/dev/null`; do
      bsp_cfg=`echo "$bsp_cfgs" | sed \
        -e "s,^${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}/$bsp_family/config/,," \
        -e "s,\.cfg$,,"`
      $1="[$]$1 $bsp_cfg"
    done
  done
  AS_IF([test -z "[$]$1"],
    [AC_MSG_RESULT([none])],
    [AC_MSG_RESULT([$]$1)])
])dnl
