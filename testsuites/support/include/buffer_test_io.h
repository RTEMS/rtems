/*
 *  Support for running the test output through a buffer
 */

#ifndef __BUFFER_TEST_IO_h
#define __BUFFER_TEST_IO_h

#include <rtems/test.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_BEGIN() rtems_test_begin(rtems_test_name, TEST_STATE)
#define TEST_END()   rtems_test_end(rtems_test_name)

#ifdef __cplusplus
};
#endif

#endif
