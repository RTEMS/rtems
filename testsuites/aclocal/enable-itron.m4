dnl $Id$

AC_DEFUN(RTEMS_ENABLE_ITRON,
[
## AC_BEFORE([$0], [RTEMS_CHECK_ITRON_API])dnl

AC_ARG_ENABLE(itron,
[AC_HELP_STRING([--enable-itron],[enable itron interface])],
[case "${enableval}" in 
  yes) RTEMS_HAS_ITRON_API=yes ;;
  no) RTEMS_HAS_ITRON_API=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-itron option) ;;
esac],[RTEMS_HAS_ITRON_API=yes]) 

case "${host}" in
  # hpux unix port should go here
  i[[34567]]86-pc-linux*)         # unix "simulator" port
	RTEMS_HAS_ITRON_API=no
	;;
  i[[34567]]86-*freebsd*) # unix "simulator" port
	RTEMS_HAS_ITRON_API=no
	;;
  no_cpu-*rtems*)
	RTEMS_HAS_ITRON_API=no
	;;
  sparc-sun-solaris*)             # unix "simulator" port
	RTEMS_HAS_ITRON_API=no
	;;
  *) 
	;;
esac
AC_SUBST(RTEMS_HAS_ITRON_API)
])
