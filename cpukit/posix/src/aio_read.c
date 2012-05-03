/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com> 
 * 
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */


#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <aio.h>
#include <errno.h>
#include <fcntl.h>
#include <rtems/posix/aio_misc.h>
#include <rtems/system.h>
#include <rtems/seterr.h>
#include <stdlib.h>

/*
 *  aio_read
 *
 * Asynchronous write to a file
 *
 *  Input parameters:
 *        aiocbp - asynchronous I/O control block
 *
 *  Output parameters:
 *        -1 - request could not pe enqueued
 *           - FD not opened for write
 *           - invalid aio_reqprio or aio_offset or
 *             aio_nbytes
 *           - not enough memory
 *         0 - otherwise
 */

int
aio_read (struct aiocb *aiocbp)
{
  rtems_aio_request *req;
  int mode;

  mode = fcntl (aiocbp->aio_fildes, F_GETFL);
  if (!(((mode & O_ACCMODE) == O_RDONLY) || ((mode & O_ACCMODE) == O_RDWR)))
    rtems_aio_set_errno_return_minus_one (EBADF, aiocbp);
  
  if (aiocbp->aio_reqprio < 0 || aiocbp->aio_reqprio > AIO_PRIO_DELTA_MAX)
    rtems_aio_set_errno_return_minus_one (EINVAL, aiocbp);
  
  if (aiocbp->aio_offset < 0)
    rtems_aio_set_errno_return_minus_one (EINVAL, aiocbp);

  req = malloc (sizeof (rtems_aio_request));
  if (req == NULL)
    rtems_aio_set_errno_return_minus_one (EAGAIN, aiocbp);

  req->aiocbp = aiocbp;
  req->aiocbp->aio_lio_opcode = LIO_READ;

  return rtems_aio_enqueue (req);
}
