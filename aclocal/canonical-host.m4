dnl $Id$

AC_DEFUN(RTEMS_CANONICAL_HOST,
[dnl
AC_REQUIRE([AC_CANONICAL_HOST])
RTEMS_HOST=$host_os
case "${target}" in
  # hpux unix port should go here
  i[[34567]]86-*linux*)          # unix "simulator" port
        RTEMS_HOST=Linux
	;;
  i[[34567]]86-*freebsd*)         # unix "simulator" port
        RTEMS_HOST=FreeBSD
	;;
  i[[34567]]86-pc-cygwin*) 	# Cygwin is just enough unix like :)
        RTEMS_HOST=Cygwin
	;;
  sparc-sun-solaris*)           # unix "simulator" port
        RTEMS_HOST=Solaris
	;;
  *) 
	;;
esac
AC_SUBST(RTEMS_HOST)
])dnl
