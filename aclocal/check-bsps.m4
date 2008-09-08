dnl $Id$

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
    case $bsp_family in
        # Now account for BSPs with build variants
          c4xsim)            bsps="c4xsim c3xsim";;
          gen68360)          bsps="gen68360 gen68360_040 pgh360";;
          tqm8xx)            bsps="pghplus tqm8xx_stk8xx";;
          genmcf548x)        bsps="m5484FireEngine";;
          p4000)             bsps="p4600 p4650";;
          mvme162)           bsps="mvme162 mvme162lx";;
          mbx8xx)            bsps="mbx821_001 mbx860_001b"
                             bsps="$bsps mbx821_002 mbx821_002b"
                             bsps="$bsps mbx860_002"
                             bsps="$bsps mbx860_005b"
                             ;;
          gen5200)           bsps="pm520_cr825 pm520_ze30 brs5l";;
          mpc55xxevb)        bsps="mpc5566evb";;
          gen83xx)           bsps="mpc8349eamds hsc_cm01 mpc8313erdb";;
	  motorola_powerpc)  bsps="mvme2307 mcp750 mtx603e mvme2100";;
	  pc386)             bsps="pc386 pc386dx pc486 pc586 pc686 pck6";;
          erc32)             bsps="erc32 sis";;
          rtl22xx)           bsps="rtl22xx rtl22xx_t)";;
	  sim68000)          bsps="sim68000 simcpu32";;
	  shsim)             bsps="simsh7032 simsh7045";;
	  *) 		     bsps="$bsp_family";;
    esac;
    $1="[$]$1 $bsps"
  done
  AS_IF([test -z "[$]$1"],
    [AC_MSG_RESULT([none])],
    [AC_MSG_RESULT([$]$1)])
])dnl
