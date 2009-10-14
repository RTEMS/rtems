dnl
dnl $Id$
dnl

dnl canonicalize target cpu
dnl NOTE: Most rtems targets do not fullfil autoconf's
dnl target naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN([RTEMS_CANONICAL_TARGET_CPU],
[
AC_CANONICAL_HOST
AC_MSG_CHECKING(rtems target cpu)
case "${host}" in
  no_cpu-*rtems*)
        RTEMS_CPU=no_cpu
	RTEMS_HOST=$host_os
	;;
  *) 
	RTEMS_CPU=`echo $host | sed 's%^\([[^-]]*\)-\(.*\)$%\1%'`
	RTEMS_HOST=$host_os
	;;
esac
AC_SUBST(RTEMS_CPU)
AC_SUBST(RTEMS_HOST)
AC_MSG_RESULT($RTEMS_CPU)
])
