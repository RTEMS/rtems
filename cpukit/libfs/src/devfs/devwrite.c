/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/io.h>

#include "devfs.h"

ssize_t devFS_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  rtems_libio_rw_args_t   args;
  rtems_status_code       status;
  const devFS_node *np = iop->pathinfo.node_access;

  args.iop         = iop;
  args.offset      = iop->offset;
  args.buffer      = (void *) buffer;
  args.count       = count;
  args.flags       = iop->flags;
  args.bytes_moved = 0;

  status = rtems_io_write(
    np->major,
    np->minor,
    (void *) &args
  );

  if ( status )
    return rtems_deviceio_errno(status);

  return (ssize_t) args.bytes_moved;
}

