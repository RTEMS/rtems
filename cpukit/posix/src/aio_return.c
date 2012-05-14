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

#include <rtems/system.h>
#include <rtems/seterr.h>

/*
 *  aio_return
 *
 * Retrieve return status of an asynchronous I/O operation
 *
 *  Input parameters:
 *        aiocbp - asynchronous I/O control block
 *
 *  Output parameters:
 *        aiocbp->return_value
 */

ssize_t
aio_return (const struct aiocb *aiocbp)
{
  return aiocbp->return_value;
}
