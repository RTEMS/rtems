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
AC_REQUIRE([AC_CANONICAL_SYSTEM])
AC_MSG_CHECKING(rtems target cpu)
changequote(,)dnl
case "${target}" in
  # hpux unix port should go here
  i[3456]86-go32-rtems*)
	target_cpu=i386
	;;
  i[3456]86-pc-linux*)		# unix "simulator" port
	target_cpu=unix
	;;
  i[3456]86-*freebsd2*) 	# unix "simulator" port
	target_cpu=unix
	;;
  no_cpu-*rtems*)
        target_cpu=no_cpu
	;;
  ppc*-*rtems*)
	target_cpu=powerpc
	;;
  sparc-sun-solaris*)           # unix "simulator" port
	target_cpu=unix
	;;
  *) 
	target_cpu=`echo $target | sed 's%^\([^-]*\)-\(.*\)$%\1%'`
	;;
esac
changequote([,])dnl
AC_MSG_RESULT($target_cpu)
])
