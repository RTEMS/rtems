/**
 * @file
 *
 * @brief Maps Open Operation to rtems_io_open
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

int devFS_open(
  rtems_libio_t *iop,
  const char    *pathname,
  int            oflag,
  mode_t         mode
)
{
  const devFS_node *np = iop->pathinfo.node_access;

  return rtems_deviceio_open(
    iop,
    pathname,
    oflag,
    mode,
    np->major,
    np->minor
  );
}
