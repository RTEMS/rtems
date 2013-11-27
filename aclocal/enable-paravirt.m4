AC_DEFUN([RTEMS_ENABLE_PARAVIRT],
[

AC_ARG_ENABLE(paravirt,
[AS_HELP_STRING([--enable-paravirt],[enable support for paravirtualization 
(i386 only) (default=no) ])],

[case "${enableval}" in 
  yes) case "${RTEMS_CPU}" in
         i386)	    RTEMS_HAS_PARAVIRT=yes ;;
         *)         RTEMS_HAS_PARAVIRT=no ;;
       esac
       ;;
  no) RTEMS_HAS_PARAVIRT=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-paravirt option) ;;
esac],[RTEMS_HAS_PARAVIRT=no]) 
])
