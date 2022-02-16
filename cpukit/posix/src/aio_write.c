/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Function queues I/O request described by buffer pointed by aiocb 
 */

/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com> 
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
#include <fcntl.h>
#include <rtems/posix/aio_misc.h>
#include <rtems/seterr.h>
#include <stdlib.h>
#include <limits.h>

/*
 *  aio_write
 *
 * Asynchronous write to a file
 *
 *  Input parameters:
 *        aiocbp - asynchronous I/O control block
 *
 *  Output parameters:
 *        -1 - request could not be enqueued
 *           - FD not opened for write
 *           - invalid aio_reqprio or aio_offset or
 *             aio_nbytes
 *           - not enough memory
 *         0 - otherwise
 */

int
aio_write (struct aiocb *aiocbp)
{
  rtems_aio_request *req;
  int mode;

  mode = fcntl (aiocbp->aio_fildes, F_GETFL);
  if (!(((mode & O_ACCMODE) == O_WRONLY) || ((mode & O_ACCMODE) == O_RDWR)))
    rtems_aio_set_errno_return_minus_one (EBADF, aiocbp);

  if (aiocbp->aio_reqprio < 0 || aiocbp->aio_reqprio > AIO_PRIO_DELTA_MAX)
    rtems_aio_set_errno_return_minus_one (EINVAL, aiocbp);

  if (aiocbp->aio_offset < 0)
    rtems_aio_set_errno_return_minus_one (EINVAL, aiocbp);

  req = malloc (sizeof (rtems_aio_request));
  if (req == NULL)
    rtems_aio_set_errno_return_minus_one (EAGAIN, aiocbp);

  req->aiocbp = aiocbp;
  req->aiocbp->aio_lio_opcode = LIO_WRITE;

  return rtems_aio_enqueue (req);
}
