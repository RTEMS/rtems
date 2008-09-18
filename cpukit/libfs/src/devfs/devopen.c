/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/io.h>

#include "devfs.h"

int devFS_open(
  rtems_libio_t *iop,
  const char    *pathname,
  uint32_t       flag,
  uint32_t       mode
)
{
  rtems_libio_open_close_args_t  args;
  rtems_status_code              status;
  rtems_device_name_t           *np;

  np         = (rtems_device_name_t *)iop->file_info;

  args.iop   = iop;
  args.flags = iop->flags;
  args.mode  = mode;

  status = rtems_io_open(
    np->major,
    np->minor,
    (void *) &args
  );
  if ( status )
    return rtems_deviceio_errno(status);

  return 0;
}
