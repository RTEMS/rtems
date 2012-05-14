AC_DEFUN([RTEMS_ENABLE_POSIX],
[
## AC_BEFORE([$0], [RTEMS_CHECK_POSIX_API])dnl

AC_ARG_ENABLE(posix,
AS_HELP_STRING(--enable-posix,enable posix interface),
[case "${enableval}" in 
  yes) RTEMS_HAS_POSIX_API=yes ;;
  no) RTEMS_HAS_POSIX_API=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-posix option) ;;
esac],[RTEMS_HAS_POSIX_API=yes]) 

case "${host}" in
  no_cpu-*rtems*)
	RTEMS_HAS_POSIX_API=no
	;;
  *) 
	;;
esac
AC_SUBST(RTEMS_HAS_POSIX_API)
])
