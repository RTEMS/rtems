/*
 *  Support for running the test output through a buffer
 */

#ifndef __BUFFER_TEST_IO_h
#define __BUFFER_TEST_IO_h

#include <rtems/test.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Test states. No state string is an expected pass.
 */
#if (TEST_STATE_EXPECTED_FAIL && TEST_STATE_USER_INPUT) || \
    (TEST_STATE_EXPECTED_FAIL && TEST_STATE_INDETERMINATE) || \
    (TEST_STATE_EXPECTED_FAIL && TEST_STATE_BENCHMARK) || \
    (TEST_STATE_USER_INPUT    && TEST_STATE_INDETERMINATE) || \
    (TEST_STATE_USER_INPUT    && TEST_STATE_BENCHMARK) || \
    (TEST_STATE_INDETERMINATE && TEST_STATE_BENCHMARK)
  #error Test states must be unique
#endif

#if TEST_STATE_EXPECTED_FAIL
  #define TEST_STATE_STRING "*** TEST STATE: EXPECTED-FAIL\n"
#elif TEST_STATE_USER_INPUT
  #define TEST_STATE_STRING "*** TEST STATE: USER_INPUT\n"
#elif TEST_STATE_INDETERMINATE
  #define TEST_STATE_STRING "*** TEST STATE: INDETERMINATE\n"
#elif TEST_STATE_BENCHMARK
  #define TEST_STATE_STRING "*** TEST STATE: BENCHMARK\n"
#endif

#undef printf
#define printf(...) \
  do { \
     rtems_printf( &rtems_test_printer, __VA_ARGS__ ); \
  } while (0)

#undef puts
#define puts(_s) \
  do { \
     rtems_printf( &rtems_test_printer, "%s\n", _s ); \
  } while (0)

#undef putchar
#define putchar(_c) \
  do { \
     rtems_printf( &rtems_test_printer, "%c", _c ); \
  } while (0)

/* Do not call exit() since it closes stdin, etc and pulls in stdio code */
#define rtems_test_exit(_s) \
  do { \
    rtems_shutdown_executive(0); \
  } while (0)

#define FLUSH_OUTPUT() \
  do { \
  } while (0)

#if defined(TEST_STATE_STRING)
  #define TEST_BEGIN() \
  do { \
    rtems_printf( &rtems_test_printer, "\n"); \
    rtems_printf( &rtems_test_printer, TEST_BEGIN_STRING ); \
    rtems_printf( &rtems_test_printer, TEST_STATE_STRING ); \
  } while (0)
#else
  #define TEST_BEGIN() \
  do { \
    rtems_printf( &rtems_test_printer, "\n" ); \
    rtems_printf( &rtems_test_printer, TEST_BEGIN_STRING ); \
  } while (0)
#endif

#define TEST_END() \
  do { \
     rtems_printf( &rtems_test_printer, "\n" ); \
     rtems_printf( &rtems_test_printer, TEST_END_STRING ); \
     rtems_printf( &rtems_test_printer, "\n" ); \
  } while (0)

#ifdef __cplusplus
};
#endif

#endif
