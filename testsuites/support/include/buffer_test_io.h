/*
 *  Support for running the test output through a buffer
 */

#ifndef __BUFFER_TEST_IO_h
#define __BUFFER_TEST_IO_h

#include <rtems/bspIo.h>
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

/*
 *  USE PRINTK TO MINIMIZE SIZE
 */
#if defined(TESTS_USE_PRINTK)

#include <rtems/print.h>

  #undef printf
  #define printf(...) \
    do { \
       printk( __VA_ARGS__); \
    } while (0)

  #undef puts
  #define puts(_s) \
    do { \
       printk( "%s\n", _s); \
    } while (0)

  #undef putchar
  #define putchar(_c) \
    do { \
       printk( "%c", _c ); \
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
      printk("\n"); \
      printk(TEST_BEGIN_STRING); \
      printk(TEST_STATE_STRING); \
    } while (0)
  #else
    #define TEST_BEGIN() \
    do { \
      printk("\n"); \
      printk(TEST_BEGIN_STRING); \
    } while (0)
  #endif

  #define TEST_END() \
    do { \
       printk( "\n" ); \
       printk(TEST_END_STRING); \
       printk( "\n" ); \
    } while (0)

/*
 *  BUFFER TEST OUTPUT
 */
#else

  #include <stdio.h>
  #include <stdlib.h>

  #define TEST_PRINT__FORMAT(_fmtpos, _appos) \
              __attribute__((__format__(__printf__, _fmtpos, _appos)))

  #define _TEST_OUTPUT_BUFFER_SIZE 2048

  extern char _test_output_buffer[_TEST_OUTPUT_BUFFER_SIZE];

  void _test_output_printf(const char *, ...) TEST_PRINT__FORMAT(1, 2);
  void _test_output_append(const char *);
  void _test_output_flush(void);

  #define rtems_test_exit(_s) \
    do { \
      fflush(stdout); \
      fflush(stderr); \
      _test_output_flush(); \
      exit(_s); \
    } while (0)

  #undef printf
  #define printf(...) _test_output_printf( __VA_ARGS__ )

  #undef puts
  #define puts(_string) \
    do { \
       _test_output_append( _string ); \
       _test_output_append( "\n" ); \
    } while (0)

  #undef putchar
  #define putchar(_c) \
    do { \
       char _buffer[2]; \
       _buffer[0] = _c; \
       _buffer[1] = '\0'; \
       _test_output_append( _buffer ); \
    } while (0)

  /* we write to stderr when there is a pause() */
  #define FLUSH_OUTPUT() _test_output_flush()

  #if defined(TEST_STATE_STRING)
    #define TEST_BEGIN() \
    do { \
       _test_output_append( "\n" ); \
       _test_output_printf(TEST_BEGIN_STRING); \
       _test_output_append(TEST_STATE_STRING); \
       _test_output_append( "\n" ); \
    } while (0)
  #else
    #define TEST_BEGIN() \
      do { \
         _test_output_append( "\n" ); \
         _test_output_printf(TEST_BEGIN_STRING); \
         _test_output_append( "\n" ); \
      } while (0)
  #endif

  #define TEST_END() \
    do { \
       _test_output_append( "\n" ); \
       _test_output_printf(TEST_END_STRING); \
       _test_output_append( "\n" ); \
    } while (0)

  /*
   * Inline the tests this way because adding the code to the support directory
   * requires all the makefile files to changed.
   */
  #if defined(TEST_INIT)

    char _test_output_buffer[_TEST_OUTPUT_BUFFER_SIZE];
    int _test_output_buffer_index = 0;

    void _test_output_printf(const char* format, ...)
    {
      va_list args;
      char*   in;
      size_t  size;
      bool    lf;
      va_start(args, format);
      in = _test_output_buffer + _test_output_buffer_index;
      size = vsniprintf(in,
                        _TEST_OUTPUT_BUFFER_SIZE - _test_output_buffer_index,
                        format, args);
      lf = memchr(in, '\n', size);
      _test_output_buffer_index += size;
      if ( lf || _test_output_buffer_index >= (_TEST_OUTPUT_BUFFER_SIZE - 80) )
        _test_output_flush();
      va_end(args);
    }

    void _test_output_append(const char *_buffer)
    {
      char*  in;
      size_t size;
      bool   lf;
      in = _test_output_buffer + _test_output_buffer_index;
      size = strlcpy(in, _buffer, _TEST_OUTPUT_BUFFER_SIZE - _test_output_buffer_index);
      lf = memchr(in, '\n', size);
      if ( lf || _test_output_buffer_index >= (_TEST_OUTPUT_BUFFER_SIZE - 80) )
        _test_output_flush();
    }

    void _test_output_flush(void)
    {
      printk( _test_output_buffer );
      _test_output_buffer_index = 0;
      _test_output_buffer[0] = '\0';
    }

  #endif

#endif

#ifdef __cplusplus
};
#endif

#endif
