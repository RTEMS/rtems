/*
 *  Support for running the test output through a buffer
 */

#ifndef __BUFFER_TEST_IO_h
#define __BUFFER_TEST_IO_h

#include <rtems/test.h>

#ifdef __cplusplus
extern "C" {
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

#define TEST_BEGIN() rtems_test_begin(rtems_test_name, TEST_STATE)
#define TEST_END()   rtems_test_end(rtems_test_name)

#ifdef __cplusplus
};
#endif

#endif
