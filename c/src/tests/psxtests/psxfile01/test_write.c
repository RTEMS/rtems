/*
 *  A test support function which performs a write() and
 *  handles implied open(), lseek(), write(), and close() operations.
 *
 *  $Id$
 */

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <assert.h>

#include <pmacros.h>

/*
 *  test_write routine
 */

void test_write(
  char   *file,
  off_t  offset,
  char  *buffer
)
{
  int   fd;
  int   status;
  int   length;


  length = strlen( buffer );

  fd = open( file, O_WRONLY );
  if ( fd == -1 ) {
    printf( "test_write: open( %s ) failed : %s\n", file, strerror( errno ) );
    rtems_test_exit( 0 );
  }

  status = lseek( fd, offset, SEEK_SET );
  assert( status != -1 );

  status = write( fd, buffer, length );
  if ( status == -1 ) {
    printf( "test_write: write( %s ) failed : %s\n", file, strerror( errno ) );
    rtems_test_exit( 0 );
  }

  if ( status != length ) {
    printf( "test_write: write( %s ) only wrote %d of %d bytes\n",
            file, status, length );
    rtems_test_exit( 0 );
  }

  status = close( fd );
  assert( !status );
}
