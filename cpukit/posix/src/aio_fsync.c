/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIX_AIO
 *
 * @brief Syncing of all Outstanding Asynchronous I/O Operations.
 */

/*
 *  Copyright 2010, Alin Rus <alin.codejunkie@gmail.com>
 *  Copyright 2024, Alessandro Nardin <ale.daluch@gmail.com>
 * 
 *  COPYRIGHT (c) 1989-2011, 2024.
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

#include <aio.h>
#include <errno.h>
#include <stdlib.h>
#include <rtems/posix/aio_misc.h>
#include <rtems/seterr.h>

/*
 * Coverity 1616108 reported that there is dead code based on checking
 * both O_DSYNC and O_SYNC when they have the same value. Adding this
 * #error so if they ever are not equal, we will immediately know and
 * fix the code. It now assumes they are equal.
 */
#if (O_DSYNC != O_SYNC)
  #error "Implementation assumes O_DSYNC == O_SYNC. Update if this changes."
#endif

int aio_fsync(
  int            op,
  struct aiocb  *aiocbp
)
{
  rtems_aio_request *req;
  int mode;


  if ( 1 + atomic_load( &aio_request_queue.queued_requests ) > RTEMS_AIO_MAX ) {
    rtems_set_errno_and_return_minus_one( EAGAIN );
  }

  if ( aiocbp == NULL )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   * If O_SYNC != O_DSYNC, then this code needs to check for each individually.
   */
  if ( op != O_DSYNC )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( rtems_aio_check_sigevent( &aiocbp->aio_sigevent ) == 0 )
    rtems_set_errno_and_return_minus_one( EINVAL );
  
  mode = fcntl( aiocbp->aio_fildes, F_GETFL );
  if (
    !(
      ((mode & O_ACCMODE) == O_WRONLY) ||
      ((mode & O_ACCMODE) == O_RDWR)
    )
  )
    rtems_set_errno_and_return_minus_one( EBADF );

  req = (rtems_aio_request *) calloc( 1, sizeof( *req ) );
  if ( req == NULL )
    rtems_set_errno_and_return_minus_one( EAGAIN );

  /*
   * If O_SYNC != O_DSYNC, then this code needs to check for each individually.
   */
  req->aiocbp = aiocbp;
  req->op_type = AIO_OP_SYNC;
  req->listcbp = NULL;
  
  return rtems_aio_enqueue( req );
}

