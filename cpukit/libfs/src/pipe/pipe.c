/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup FIFO_PIPE
 *
 * @brief This source file contains the implementation of pipe(), pipe2(),
 *   rtems_filesystem_register_pipe() and rtems_filesystem_register_pipe2().
 */

/*
 * Copyright (C) 2008 Wei Shen <jshen.yn@gmail.com>
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

#define _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <rtems/libio_.h>
#include <rtems/libio.h>
#include <rtems/seterr.h>
#include <rtems/pipe.h>

/* Incremental number added to names of anonymous pipe files */
/* FIXME: This approach is questionable */
static uint16_t rtems_pipe_no = 0;

static int pipe_default( int filsdes[ 2 ] )
{
  rtems_libio_t *iop;
  int            err = 0;

  if ( filsdes == NULL ) {
    rtems_set_errno_and_return_minus_one( EFAULT );
  }

  if ( rtems_mkdir( "/tmp", S_IRWXU | S_IRWXG | S_IRWXO ) != 0 ) {
    return -1;
  }

  /* /tmp/.fifoXXXX */
  char fifopath[ 15 ];
  memcpy( fifopath, "/tmp/.fifo", 10 );
  sprintf( fifopath + 10, "%04x", rtems_pipe_no++ );

  /* Try creating FIFO file until find an available file name */
  while ( mkfifo( fifopath, S_IRUSR | S_IWUSR ) != 0 ) {
    if ( errno != EEXIST ) {
      return -1;
    }
    /* Just try once... */
    return -1;
    /* sprintf(fifopath + 10, "%04x", rtems_pipe_no ++); */
  }

  /* Non-blocking open to avoid waiting for writers */
  filsdes[ 0 ] = open( fifopath, O_RDONLY | O_NONBLOCK );
  if ( filsdes[ 0 ] < 0 ) {
    err = errno;
    /* Delete file at errors, or else if pipe is successfully created
     the file node will be deleted after it is closed by all. */
    unlink( fifopath );
  } else {
    /* Reset open file to blocking mode */
    iop = rtems_libio_iop( filsdes[ 0 ] );
    rtems_libio_iop_flags_clear( iop, LIBIO_FLAGS_NO_DELAY );

    filsdes[ 1 ] = open( fifopath, O_WRONLY );

    if ( filsdes[ 1 ] < 0 ) {
      err = errno;
      close( filsdes[ 0 ] );
    }
    unlink( fifopath );
  }
  if ( err != 0 ) {
    rtems_set_errno_and_return_minus_one( err );
  }
  return 0;
}

static int pipe2_default( int fildes[ 2 ], int flags )
{
  (void) fildes;
  (void) flags;
  errno = ENXIO;
  return -1;
}

static rtems_pipe_handler_t  pipe_handler = pipe_default;
static rtems_pipe2_handler_t pipe2_handler = pipe2_default;

void rtems_filesystem_register_pipe( rtems_pipe_handler_t phandler )
{
  rtems_libio_lock();
  pipe_handler = phandler;
  rtems_libio_unlock();
}

void rtems_filesystem_register_pipe2( rtems_pipe2_handler_t p2handler )
{
  rtems_libio_lock();
  pipe2_handler = p2handler;
  rtems_libio_unlock();
}

int pipe( int fildes[ 2 ] )
{
  rtems_pipe_handler_t phandler;
  rtems_libio_lock();
  phandler = pipe_handler;
  rtems_libio_unlock();
  return phandler( fildes );
}

int pipe2( int fildes[ 2 ], int flags )
{
  rtems_pipe2_handler_t p2handler;
  rtems_libio_lock();
  p2handler = pipe2_handler;
  rtems_libio_unlock();
  return p2handler( fildes, flags );
}
