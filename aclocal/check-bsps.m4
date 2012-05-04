dnl Report all available bsps for a target within the source tree
dnl
dnl RTEMS_CHECK_BSPS(bsp_list)
AC_DEFUN([RTEMS_CHECK_BSPS],
[
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_TOP])dnl sets RTEMS_TOPdir

AC_MSG_CHECKING([for available BSPs])
  $1=
  for bsp_spec in `ls "$srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU"/*/bsp_specs 2>/dev/null`; do
    bsp_family=`echo "$bsp_spec" | sed \
      -e "s,^$srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU/,," \
      -e "s,/bsp_specs$,,"`
    for bsp_cfgs in `ls "$srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU/$bsp_family/make/custom/"*.cfg 2>/dev/null`; do
      bsp_cfg=`echo "$bsp_cfgs" | sed \
        -e "s,^$srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU/$bsp_family/make/custom/,," \
        -e "s,\.cfg$,,"`
      $1="[$]$1 $bsp_cfg"
    done
  done
  AS_IF([test -z "[$]$1"],
    [AC_MSG_RESULT([none])],
    [AC_MSG_RESULT([$]$1)])
])dnl
