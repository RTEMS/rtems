dnl $Id$

dnl Report all available bsps for a target,
dnl check if a bsp-subdirectory is present for all bsps found
dnl
dnl RTEMS_CHECK_BSPS(bsp_list)
AC_DEFUN(RTEMS_CHECK_BSPS,
[
AC_REQUIRE([RTEMS_CHECK_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_TOP])dnl sets RTEMS_TOPdir
AC_MSG_CHECKING([for bsps])
    files=`ls $srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU`
    for file in $files; do
      case $file in
        ChangeLog*);;
        shared*);;
        Makefile*);;
        READ*);;
        CVS*);;
        pxfl*);;
        ac*);;
        config*);;
        # Now account for BSPs with build variants
        c4xsim)            rtems_bsp="$rtems_bsp c4xsim c3xsim";;
        gen68360)          rtems_bsp="$rtems_bsp gen68360 gen68360_040";;
        p4000)             rtems_bsp="$rtems_bsp p4600 p4650";;
        mvme162)           rtems_bsp="$rtems_bsp mvme162 mvme162lx";;
        mbx8xx)            rtems_bsp="$rtems_bsp mbx821_001 mbx860_002";;
	motorola_powerpc)  rtems_bsp="$rtems_bsp mvme2307 mcp750";;
	pc386)             rtems_bsp="$rtems_bsp pc386 pc386dx pc486 pc586 pc686 pck6";;
	erc32)             rtems_bsp="$rtems_bsp erc32 erc32nfp";;
	leon)              rtems_bsp="$rtems_bsp leon1 leon2";;
	sim68000)          rtems_bsp="$rtems_bsp sim68000 simcpu32";;
	shsim)             rtems_bsp="$rtems_bsp simsh7032 simsh7045";;
	*) if test -d $srcdir/$RTEMS_TOPdir/c/src/lib/libbsp/$RTEMS_CPU/$file; then
	     $1="[$]$1 $file"
	   fi;;
      esac;
    done
AC_MSG_RESULT([[$]$1 .. done])
])dnl

AC_DEFUN(RTEMS_CHECK_CUSTOM_BSP,
[dnl
AC_REQUIRE([RTEMS_TOP])

AC_MSG_CHECKING([for make/custom/[$]$1.cfg])
if test -r "$srcdir/$RTEMS_TOPdir/make/custom/[$]$1.cfg"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_ERROR([no])
fi
])dnl
