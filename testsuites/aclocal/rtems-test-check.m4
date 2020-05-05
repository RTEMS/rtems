dnl
dnl Check a test getting it's status.
dnl
AC_DEFUN([RTEMS_TEST_CHECK],
[AC_REQUIRE([RTEMS_ENV_RTEMSBSP])
 AC_REQUIRE([RTEMS_SOURCE_TOP])
 AC_MSG_CHECKING([${RTEMS_CPU}/${RTEMS_BSP} $1 test])
 tcheck="${RTEMS_SOURCE_ROOT}/testsuites/rtems-test-check"
 tdata="${RTEMS_BSP}-testsuite.tcfg"
 tincludes="${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}:${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}:${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}/config:${RTEMS_SOURCE_ROOT}/testsuites"
 if test -f $tcheck; then
  check_result=`$tcheck exclude ${RTEMS_BSP} $tdata $tincludes $1`
 else
  check_result=$1
 fi
 if test "$1" = "$check_result"; then
  if test -f $tcheck; then
   test_CFLAGS=`$tcheck cflags ${RTEMS_BSP} $tdata $tincludes $1`
  fi
  if test -z "$test_CFLAGS"; then
   result_msg="PASS"
  else
   result_msg="$test_CFLAGS"
  fi
 else
   result_msg="EXCLUDED"
 fi
 AC_MSG_RESULT([$result_msg])
 AM_CONDITIONAL([TEST_$1], [test "$result_msg" != "EXCLUDED"])
 AC_SUBST([TEST_FLAGS_$1], [$test_CFLAGS])
])
