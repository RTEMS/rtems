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

ssize_t devFS_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  const devFS_node *np = iop->pathinfo.node_access;

  return rtems_deviceio_write( iop, buffer, count, np->major, np->minor );
}
