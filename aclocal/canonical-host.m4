dnl $Id$

AC_DEFUN(RTEMS_CANONICAL_HOST,
[dnl
AC_REQUIRE([AC_CANONICAL_HOST])
RTEMS_HOST=$host_os
changequote(,)dnl
case "${target}" in
  # hpux unix port should go here
  i[3456]86-*linux*)          # unix "simulator" port
        RTEMS_HOST=Linux
	;;
  i[3456]86-*freebsd2*)         # unix "simulator" port
        RTEMS_HOST=FreeBSD
	;;
  i[3456]86-pc-cygwin*) 	# Cygwin is just enough unix like :)
        RTEMS_HOST=Cygwin
	;;
  sparc-sun-solaris*)           # unix "simulator" port
        RTEMS_HOST=Solaris
	;;
  *) 
	;;
esac
changequote([,])dnl
AC_SUBST(RTEMS_HOST)
])dnl
