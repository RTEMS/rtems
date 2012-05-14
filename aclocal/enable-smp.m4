AC_DEFUN([RTEMS_ENABLE_SMP],
[
## AC_BEFORE([$0], [RTEMS_CHECK_SMP])dnl

AC_ARG_ENABLE(smp,
[AS_HELP_STRING([--enable-smp],[enable smp interface])],
[case "${enableval}" in 
  yes) case "${RTEMS_CPU}" in
         sparc|i386) RTEMS_HAS_SMP=yes ;;
         *)          RTEMS_HAS_SMP=no ;;
       esac
       ;;
  no) RTEMS_HAS_SMP=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-smp option) ;;
esac],[RTEMS_HAS_SMP=no]) 
])
