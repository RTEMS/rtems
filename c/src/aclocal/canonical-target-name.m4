dnl
dnl $Id$
dnl

dnl canonicalize target cpu
dnl NOTE: Most rtems targets do not fullfil autoconf's
dnl target naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN(RTEMS_CANONICAL_TARGET_CPU,
[
AC_CANONICAL_HOST
AC_MSG_CHECKING(rtems target cpu)
case "${host}" in
  # hpux unix port should go here
  i[[34567]]86-*linux*)		# unix "simulator" port
	RTEMS_CPU=unix
        RTEMS_HOST=Linux
	;;
  i[[34567]]86-*freebsd*) 	# unix "simulator" port
	RTEMS_CPU=unix
        RTEMS_HOST=FreeBSD
	;;
  i[[34567]]86-pc-cygwin*) 	# Cygwin is just enough unix like :)
	RTEMS_CPU=unix
        RTEMS_HOST=Cygwin
	;;
  no_cpu-*rtems*)
        RTEMS_CPU=no_cpu
	RTEMS_HOST=$host_os
	;;
  sparc-sun-solaris*)           # unix "simulator" port
	RTEMS_CPU=unix
        RTEMS_HOST=Solaris
	;;
  tic4x-*rtems*)          	# gcc changed the name
	RTEMS_CPU=c4x
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
