/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "devfs.h"

#include <rtems/deviceio.h>

int devFS_ioctl(
  rtems_libio_t   *iop,
  ioctl_command_t  command,
  void            *buffer
)
{
  const devFS_node *np = iop->pathinfo.node_access;

  return rtems_deviceio_control( iop, command, buffer, np->major, np->minor );
}
