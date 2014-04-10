AC_DEFUN([RTEMS_ENABLE_PARAVIRT],
[

AC_ARG_ENABLE(paravirt,
[AS_HELP_STRING([--enable-paravirt],[enable support for paravirtualization
(default=no)])],

[case "${enableval}" in 
  yes) RTEMS_HAS_PARAVIRT=yes ;;
  no) RTEMS_HAS_PARAVIRT=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-paravirt option) ;;
esac],[RTEMS_HAS_PARAVIRT=no]) 
])
