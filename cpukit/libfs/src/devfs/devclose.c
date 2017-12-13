/**
 * @file
 *
 * @brief Maps Close Operation to rtems_io_close
 * @ingroup DevFsDeviceTable Define Device Table Type
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/devfs.h>

#include <rtems/deviceio.h>

int devFS_close(
  rtems_libio_t *iop
)
{
  const devFS_node *np = iop->pathinfo.node_access;

  return rtems_deviceio_close( iop, np->major, np->minor );
}
