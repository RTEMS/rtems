AC_DEFUN([RTEMS_ENABLE_MULTIPROCESSING],
[
AC_ARG_ENABLE(multiprocessing,
[AS_HELP_STRING(--enable-multiprocessing,enable multiprocessing interface)],
[case "${enable_multiprocessing}" in 
  yes) ;;
  no) ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-multiprocessing option) ;;
esac],[enable_multiprocessing=no])
])
