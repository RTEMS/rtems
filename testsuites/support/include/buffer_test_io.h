/*
 *  Support for running the test output through a buffer
 * 
 *  $Id$
 */

#ifndef __BUFFER_TEST_IO_h
#define __BUFFER_TEST_IO_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/*
 *  Uncomment this to get buffered test output.  When commented out,
 *  test output behaves as it always has and is printed ASAP.
 */

/* #define TESTS_BUFFER_OUTPUT */

#if !defined(TESTS_BUFFER_OUTPUT)

#undef printf
#define printf(...) \
  do { \
     iprintf( __VA_ARGS__); \ 
  } while (0)

#define rtems_test_exit(_s) \
  do { \
    exit(_s); \
  } while (0)

#define FLUSH_OUTPUT() \
  do { \
    fflush(stdout); \
  } while (0)

#else  /* buffer test output */

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

#endif  /* TEST_INIT */
#endif /* TESTS_BUFFER_OUTPUT */

#ifdef __cplusplus
}
#endif

#endif
