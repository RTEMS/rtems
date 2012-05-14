/**
 *  @file
 *
 *  A test support function which performs a write() and
 *  handles implied open(), lseek(), write(), and close() operations.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <pmacros.h>

/* forward declarations to avoid warnings */
void test_write(char *file, off_t offset, char *buffer);

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
  rtems_test_assert( status != -1 );

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
  rtems_test_assert( !status );
}
