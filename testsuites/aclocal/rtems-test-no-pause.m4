AC_DEFUN([RTEMS_ENABLE_RTEMS_TEST_NO_PAUSE],
[AC_ARG_ENABLE(test-no-pause,
AS_HELP_STRING(--disable-test-no-pause,disable RTEMS_TEST_NO_PAUSE),
[case "${enableval}" in
  yes) RTEMS_TEST_NO_PAUSE=yes ;;
  no) RTEMS_TEST_NO_PAUSE=no ;;
  *) AC_MSG_ERROR([bad value ${enableval} for RTEMS_TEST_NO_PAUSE]) ;;
esac],[RTEMS_TEST_NO_PAUSE=yes])
])

AC_DEFUN([RTEMS_CHECK_RTEMS_TEST_NO_PAUSE],
[AC_REQUIRE([RTEMS_ENABLE_RTEMS_TEST_NO_PAUSE])
if test x"${RTEMS_TEST_NO_PAUSE}" = x"yes";
then
  AC_DEFINE_UNQUOTED(RTEMS_TEST_NO_PAUSE,1,[if RTEMS_TEST_NO_PAUSE is enabled])
fi
])

