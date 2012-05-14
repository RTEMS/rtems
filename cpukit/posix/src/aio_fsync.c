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
#include <stdlib.h>
#include <rtems/posix/aio_misc.h>
#include <rtems/system.h>
#include <rtems/seterr.h>

/*
 *  aio_fsync
 *
 * Asynchronous file synchronization
 *
 *  Input parameters:
 *        op     - O_SYNC
 *        aiocbp - asynchronous I/O control block
 *
 *  Output parameters:
 *        -1 - request could not pe enqueued
 *           - FD not opened for write
 *           - not enough memory
 *           - op is not O_SYNC
 *         0 - otherwise
 */

int aio_fsync(
  int            op,
  struct aiocb  *aiocbp
)
{
  rtems_aio_request *req;
  int mode;

  if (op != O_SYNC)
    rtems_aio_set_errno_return_minus_one (EINVAL, aiocbp);
  
  mode = fcntl (aiocbp->aio_fildes, F_GETFL);
  if (!(((mode & O_ACCMODE) == O_WRONLY) || ((mode & O_ACCMODE) == O_RDWR)))
    rtems_aio_set_errno_return_minus_one (EBADF, aiocbp);

  req = malloc (sizeof (rtems_aio_request));
  if (req == NULL)
    rtems_aio_set_errno_return_minus_one (EAGAIN, aiocbp);

  req->aiocbp = aiocbp;
  req->aiocbp->aio_lio_opcode = LIO_SYNC; 
  
  return rtems_aio_enqueue (req);
    
}
