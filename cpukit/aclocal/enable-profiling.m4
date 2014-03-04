AC_DEFUN([RTEMS_ENABLE_PROFILING],
  [AC_ARG_ENABLE(profiling,
    [AS_HELP_STRING([--enable-profiling],[enable support for profiling (default=no)])],
    [case "${enableval}" in 
      yes) RTEMS_HAS_PROFILING=yes ;;
      no) RTEMS_HAS_PROFILING=no ;;
      *) AC_MSG_ERROR(bad value ${enableval} for enable profiling option) ;;
    esac],
    [RTEMS_HAS_PROFILING=no])])
