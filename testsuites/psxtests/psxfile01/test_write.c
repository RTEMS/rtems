/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
