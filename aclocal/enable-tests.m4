dnl $Id$

AC_DEFUN(RTEMS_ENABLE_TESTS,
[
# If the tests are enabled, then find all the test suite Makefiles
AC_MSG_CHECKING([if the test suites are enabled? ])
AC_ARG_ENABLE(tests,
AC_HELP_STRING([--enable-tests],[enable tests (default:disabled)]),
  [case "${enableval}" in
    yes) tests_enabled=yes ;;
    no)  tests_enabled=no ;;
    *)   AC_MSG_ERROR(bad value ${enableval} for tests option) ;;
  esac], [tests_enabled=no])
AC_MSG_RESULT([$tests_enabled])
])
