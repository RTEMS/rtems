#if HAVE_CONFIG_H
/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "config.h"
#endif

#include <rtems.h>
#include <rtems/io.h>

#include "devfs.h"

int devFS_ioctl(
  rtems_libio_t *iop,
  uint32_t       command,
  void          *buffer
)
{
  rtems_libio_ioctl_args_t  args;
  rtems_status_code         status;
  rtems_driver_name_t      *np;

  np           = (rtems_device_name_t *)iop->file_info;

  args.iop     = iop;
  args.command = command;
  args.buffer  = buffer;

  status = rtems_io_control(
    np->major,
    np->minor,
    (void *) &args
  );

  if ( status )
    return rtems_deviceio_errno(status);

  return args.ioctl_return;
}

