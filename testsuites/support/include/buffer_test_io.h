/*
 *  Support for running the test output through a buffer
 */

#ifndef __BUFFER_TEST_IO_h
#define __BUFFER_TEST_IO_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Uncomment this to get buffered test output.  When commented out,
 *  test output behaves as it always has and is printed using stdio.
 */

/* #define TESTS_BUFFER_OUTPUT */
#if defined(__AVR__)
#define TESTS_USE_PRINTK
#endif

/*
 *  USE PRINTK TO MINIMIZE SIZE
 */
#if defined(TESTS_USE_PRINTK)

#include <rtems/bspIo.h>

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

/*
 *  BUFFER TEST OUTPUT
 */
#elif defined(TESTS_BUFFER_OUTPUT)

  #include <stdio.h>
  #include <stdlib.h>

  #define _TEST_OUTPUT_BUFFER_SIZE 2048
  extern char _test_output_buffer[_TEST_OUTPUT_BUFFER_SIZE];
  void _test_output_append(char *);
  void _test_output_flush(void);

  #define rtems_test_exit(_s) \
    do { \
      _test_output_flush(); \
      exit(_s); \
    } while (0)

  #undef printf
  #define printf(...) \
    do { \
       char _buffer[128]; \
       sprintf( _buffer, __VA_ARGS__); \
       _test_output_append( _buffer ); \
    } while (0)

  #undef puts
  #define puts(_string) \
    do { \
       char _buffer[128]; \
       sprintf( _buffer, "%s\n", _string ); \
       _test_output_append( _buffer ); \
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

  #if defined(TEST_INIT) || defined(CONFIGURE_INIT)

    char _test_output_buffer[_TEST_OUTPUT_BUFFER_SIZE];
    int _test_output_buffer_index = 0;

    void _test_output_append(char *_buffer)
    {
      char *p;

      for ( p=_buffer ; *p ; p++ ) {
        _test_output_buffer[_test_output_buffer_index++] = *p;
        _test_output_buffer[_test_output_buffer_index]   = '\0';
        #if 0
	  if ( *p == '\n' ) {
	    fprintf( stderr, "BUFFER -- %s", _test_output_buffer );
	    _test_output_buffer_index = 0;
	   _test_output_buffer[0]   = '\0';
	  }
        #endif
	if ( _test_output_buffer_index >= (_TEST_OUTPUT_BUFFER_SIZE - 80) )
	  _test_output_flush();
      }
    }

    #include <termios.h>
    #include <unistd.h>

    void _test_output_flush(void)
    {
      fprintf( stderr, "%s", _test_output_buffer );
      _test_output_buffer_index = 0;
      tcdrain( 2 );
    }

    #endif
/*
 *  USE IPRINT
 */
#else

  #include <stdio.h>
  #include <stdlib.h>

  /* do not use iprintf if strict ansi mode */
  #if defined(_NEWLIB_VERSION) && !defined(__STRICT_ANSI__)
    #undef printf
    #define printf(...) \
      do { \
         fiprintf( stderr, __VA_ARGS__ ); \
      } while (0)
  #else
    #undef printf
    #define printf(...) \
      do { \
         fprintf( stderr, __VA_ARGS__ ); \
      } while (0)
  #endif

  #undef puts
  #define puts(_s) \
      do { \
         printf( "%s\n", _s ); \
      } while (0)

  #undef putchar
  #define putchar(_c) \
    do { \
       printf( "%c", _c ); \
    } while (0)

  #define rtems_test_exit(_s) \
    do { \
      exit(_s); \
    } while (0)

  #define FLUSH_OUTPUT() \
    do { \
      fflush(stdout); \
    } while (0)

#endif

#ifdef __cplusplus
};
#endif

#endif
