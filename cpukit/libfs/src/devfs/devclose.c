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

int devFS_close(
  rtems_libio_t *iop
)
{
  const devFS_node *np = iop->pathinfo.node_access;

  return rtems_deviceio_close( iop, np->major, np->minor );
}
