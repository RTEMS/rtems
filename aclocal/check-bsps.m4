dnl $Id$

dnl Report all available bsps for a target,
dnl check if a bsp-subdirectory is present for all bsps found
dnl
dnl RTEMS_CHECK_BSPS(bsp_list)
AC_DEFUN([RTEMS_CHECK_BSPS],
[
AC_REQUIRE([RTEMS_CHECK_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_TOP])dnl sets RTEMS_TOPdir
AC_MSG_CHECKING([for bsps])
    files=`ls $srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU`
    for bsp_family in $files; do
      if test -r $srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU/$bsp_family/bsp_specs; then
        case $bsp_family in
        # Now account for BSPs with build variants
          c4xsim)            bsps="c4xsim c3xsim";;
          gen68360)          bsps="gen68360 gen68360_040";;
          p4000)             bsps="p4600 p4650";;
          mvme162)           bsps="mvme162 mvme162lx";;
          mbx8xx)            bsps="mbx821_001 mbx860_002 mbx860_005b";;
	  motorola_powerpc)  bsps="mvme2307 mcp750 mtx603e";;
	  pc386)             bsps="pc386 pc386dx pc486 pc586 pc686 pck6";;
	  erc32)             bsps="erc32 erc32nfp";;
	  leon)              bsps="leon1 leon2";;
	  sim68000)          bsps="sim68000 simcpu32";;
	  shsim)             bsps="simsh7032 simsh7045";;
	  *) 		     bsps="$bsp_family";;
        esac;
        for bsp in $bsps; do
          AS_IF([test -r $srcdir/$RTEMS_TOPdir/make/custom/$bsp.cfg],
	    [$1="[$]$1 $bsp"])
        done
      fi
    done
AC_MSG_RESULT([[$]$1 .. done])
])dnl
