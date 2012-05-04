AC_DEFUN([RTEMS_ENABLE_TESTS],
[
AC_ARG_ENABLE(tests,
[AS_HELP_STRING(--enable-tests,enable tests (default:samples))],
  [case "${enableval}" in
    samples) enable_tests=samples;;
    yes) enable_tests=yes ;;
    no)  enable_tests=no ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-tests]) ;;
  esac], [enable_tests=samples])
])
