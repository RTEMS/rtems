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
rtems_target=$target;
case "$target" in
no_cpu*) target=$host;;
*) ;;
esac
AC_CANONICAL_SYSTEM
AC_MSG_CHECKING(rtems target cpu)
changequote(,)dnl
case "${rtems_target}" in
  # hpux unix port should go here
  i[3456]86-go32-rtems*)
	RTEMS_CPU=i386
	;;
  i[3456]86-pc-linux*)		# unix "simulator" port
	RTEMS_CPU=unix
	;;
  i[3456]86-*freebsd2*) 	# unix "simulator" port
	RTEMS_CPU=unix
	;;
  no_cpu-*rtems*)
        RTEMS_CPU=no_cpu
	;;
  sparc-sun-solaris*)           # unix "simulator" port
	RTEMS_CPU=unix
	;;
  *) 
	RTEMS_CPU=`echo $rtems_target | sed 's%^\([^-]*\)-\(.*\)$%\1%'`
	;;
esac
changequote([,])dnl
AC_SUBST(RTEMS_CPU)
AC_MSG_RESULT($RTEMS_CPU)
])
