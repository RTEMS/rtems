dnl $Id$

AC_DEFUN(RTEMS_ENABLE_TESTS,
[
# If the tests are enabled, then find all the test suite Makefiles
AC_MSG_CHECKING([if the test suites are enabled? ])
tests_enabled=yes
AC_ARG_ENABLE(tests,
[  --enable-tests                       enable tests (default:disabled)],
  [case "${enableval}" in
    yes) AC_MSG_RESULT(yes) ;;
    no)  AC_MSG_RESULT(no) ; tests_enabled=no ;;
    *)   AC_MSG_ERROR(bad value ${enableval} for tests option) ;;
  esac], AC_MSG_RESULT(no))
])
