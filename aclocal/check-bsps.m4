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
    for file in $files; do
      if test -r $srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU/$file/bsp_specs; then
        case $file in
        # Now account for BSPs with build variants
          c4xsim)            $1="[$]$1 c4xsim c3xsim";;
          gen68360)          $1="[$]$1 gen68360 gen68360_040";;
          p4000)             $1="[$]$1 p4600 p4650";;
          mvme162)           $1="[$]$1 mvme162 mvme162lx";;
          mbx8xx)            $1="[$]$1 mbx821_001 mbx860_002 mbx860_005b";;
	  motorola_powerpc)  $1="[$]$1 mvme2307 mcp750 mtx603e";;
	  pc386)             $1="[$]$1 pc386 pc386dx pc486 pc586 pc686 pck6";;
	  erc32)             $1="[$]$1 erc32 erc32nfp";;
	  leon)              $1="[$]$1 leon1 leon2";;
	  sim68000)          $1="[$]$1 sim68000 simcpu32";;
	  shsim)             $1="[$]$1 simsh7032 simsh7045";;
	  *) 		     $1="[$]$1 $file";;
        esac;
      fi
    done
AC_MSG_RESULT([[$]$1 .. done])
])dnl
