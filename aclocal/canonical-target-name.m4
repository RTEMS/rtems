dnl canonicalize target cpu
dnl NOTE: Most rtems targets do not fullfil autoconf's
dnl target naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN([RTEMS_CANONICAL_TARGET_CPU],
[
AC_CANONICAL_TARGET
AC_MSG_CHECKING(rtems target cpu)
case "${target}" in
  no_cpu-*rtems*)
        RTEMS_CPU=no_cpu
	;;
  *) 
	RTEMS_CPU=`echo $target | sed 's%^\([[^-]]*\)-\(.*\)$%\1%'`
	;;
esac
AC_SUBST(RTEMS_CPU)
AC_MSG_RESULT($RTEMS_CPU)
])
